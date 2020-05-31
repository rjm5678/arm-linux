
/* 参考drivers\input\keyboard\gpio_keys.c */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/mm.h>
#include <linux/highmem.h>
#include <linux/blkdev.h>
#include <linux/module.h>
#include <linux/backing-dev.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/signal.h>
#include <asm/uaccess.h>
#include <asm-arm/irq.h>
#include <linux/irq.h>
#include <asm-arm/arch-s3c2410/regs-gpio.h>
#include <linux/poll.h>



struct pin_desc{
	unsigned char irq;
	unsigned int pin;
	unsigned char *name;
	unsigned int key_val;
};

struct pin_desc pins_desc[4]={
	{IRQ_EINT0,  S3C2410_GPF0,  "s2", KEY_L},
	{IRQ_EINT2,  S3C2410_GPF2,  "s3", KEY_S},
	{IRQ_EINT11, S3C2410_GPG3,  "s4", KEY_ENTER},
	{IRQ_EINT19, S3C2410_GPG11, "s5", KEY_LEFTSHIFT},
};

static volatile int ev_press = 0;
static struct input_dev *buttons_dev;
static struct pin_desc *irq_pd;
static struct timer_list buttons_timer;


irqreturn_t buttons_irq(int irq, void *dev_id)
{	
	irq_pd = (struct pin_desc *)dev_id;
	mod_timer(&buttons_timer, jiffies+1);

	return IRQ_RETVAL(IRQ_HANDLED);
		
}

int buttons_timer_function()
{	
	
	struct pin_desc *pindesc = irq_pd;
	unsigned int pinval;
	
	if (!pindesc)
		return 0;
	
	pinval = s3c2410_gpio_getpin(pindesc->pin);

	if  (pinval)
	{
		/* 松开 */
		input_event(buttons_dev, EV_KEY, pindesc->key_val, 0);
		input_sync(buttons_dev);
	}
	else
	{	
		/* 按下 */
		input_event(buttons_dev, EV_KEY, pindesc->key_val, 1);
		input_sync(buttons_dev);

	}
	return 0;
}


static int buttons_init(void)
{

	int i, ret;

	/* 1. 分配一个input_dev结构体 */
	buttons_dev = input_allocate_device();


	/* 2. 设置 */
	/* 2.1 能产生哪类事件 */
	set_bit(EV_KEY, buttons_dev->evbit);
	set_bit(EV_REP, buttons_dev->evbit);

	/* 2.2 能产生这类操作里的哪些事件: L,S,ENTER,LEFTSHIT */
	set_bit(KEY_L,			 buttons_dev->keybit);
	set_bit(KEY_S,			 buttons_dev->keybit);
	set_bit(KEY_ENTER,		 buttons_dev->keybit);
	set_bit(KEY_LEFTSHIFT,	 buttons_dev->keybit);

	/* 3. 注册 */
	ret = input_register_device(buttons_dev);

	/* 4. 硬件相关的操作 */
	init_timer(&buttons_timer);
	buttons_timer.function = buttons_timer_function; 
	add_timer(&buttons_timer);

	
	for(i=0; i<4; i++)
	{
		ret = request_irq(pins_desc[i].irq,  buttons_irq, IRQT_BOTHEDGE, pins_desc[i].name, &pins_desc[i]);
	}

	return 0;
}
static void buttons_exit(void)
{
	int i;

	for(i=0; i<4; i++)
	{
		free_irq(pins_desc[i].irq, &pins_desc[0]);
	}

	input_unregister_device(buttons_dev);
}

module_init(buttons_init);

module_exit(buttons_exit);

MODULE_LICENSE("GPL");



