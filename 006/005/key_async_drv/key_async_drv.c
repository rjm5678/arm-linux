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
#include <asm/uaccess.h>
#include <asm-arm/irq.h>
#include <linux/irq.h>
#include <asm-arm/arch-s3c2410/regs-gpio.h>
#include <linux/poll.h>



static struct class *key_async_class;
static struct class_device	*key_async_drv_class;
static int major = 0;
static unsigned char key_val;
static struct fasync_struct *button_async;


unsigned int *pGPFCON = NULL; 
unsigned int *pGPFDAT = NULL; 
unsigned int *pGPGCON = NULL; 
unsigned int *pGPGDAT = NULL; 

static DECLARE_WAIT_QUEUE_HEAD(key_async_drv_waitq);
/* 中断事件标志, 中断服务程序将它置1，key_async_drv_read将它清0 */
static volatile int ev_press = 0;


struct pin_desc{
	unsigned int pin;
	unsigned int keyval;
};

struct pin_desc pins_desc[4]={
	{S3C2410_GPF0, 0x01},
	{S3C2410_GPF2, 0x02},
	{S3C2410_GPG3, 0x03},
	{S3C2410_GPG11, 0x04},
};


int buttons_irq(int irq, void *dev_id)
{	
	struct pin_desc *pindesc = (struct pin_desc *)dev_id;
	unsigned int pinval;
	
	pinval = s3c2410_gpio_getpin(pindesc->pin);

	if  (pinval)
	{
		key_val = pindesc->keyval | 0x80;
	}
	else
	{	
		key_val = pindesc->keyval;
	}
    ev_press = 1;                  /* 表示中断发生了 */
    wake_up_interruptible(&key_async_drv_waitq);   /* 唤醒休眠的进程 */	

	kill_fasync (&button_async, SIGIO, POLL_IN);	//中断程序发信号
	return 0;
}


ssize_t key_async_drv_read(struct file *file, char __user *buf, size_t size, loff_t *offset)
{
	int err;

	if (size != sizeof(key_val))
		return -EINVAL;

	wait_event_interruptible(key_async_drv_waitq, ev_press);
	
	err = copy_to_user(buf, &key_val, sizeof(key_val));	//把数据从kernel_buf读到用户空间
	ev_press = 0;
	
	return sizeof(key_val);
}

int key_async_drv_open (struct inode *node, struct file *file)
{
	int ret;

	ret = request_irq(IRQ_EINT0,  buttons_irq, IRQT_BOTHEDGE, "s2", &pins_desc[0]);
	ret = request_irq(IRQ_EINT2,  buttons_irq, IRQT_BOTHEDGE, "s3", &pins_desc[1]);
	ret = request_irq(IRQ_EINT11, buttons_irq, IRQT_BOTHEDGE, "s4", &pins_desc[2]);
	ret = request_irq(IRQ_EINT19, buttons_irq, IRQT_BOTHEDGE, "s5", &pins_desc[3]);

	return 0;
}

int key_async_drv_close(struct inode *node, struct file *file)
{

	free_irq(IRQ_EINT0, &pins_desc[0]);
	free_irq(IRQ_EINT2, &pins_desc[1]);
	free_irq(IRQ_EINT11, &pins_desc[2]);
	free_irq(IRQ_EINT19, &pins_desc[3]);

	return 0;
}

static unsigned key_async_drv_poll(struct file *file, poll_table *wait)
{
	unsigned int mask = 0;
	poll_wait(file, &key_async_drv_waitq, wait); // 不会立即休眠

	if (ev_press)
		mask |= POLLIN | POLLRDNORM;	//POLLIN = 0x01,  POLLRDNORM = 0x40

	return mask;
}

static int key_async_drv_fasync (int fd, struct file *filp, int on)
{
	printk("driver: key_async_drv_fasync\n");
	return fasync_helper (fd, filp, on, &button_async);
}



/* 定义结构体，传参给内核 */
static const struct file_operations key_async_drv_fo = {
	.owner		= THIS_MODULE,
	.open		= key_async_drv_open,
	.read		= key_async_drv_read,
	.release	= key_async_drv_close, 
	.poll		= key_async_drv_poll,
	.fasync     = key_async_drv_fasync,

};

/* 初始化（注册）驱动 */
static int __init key_async_drv_init(void)
{
	
	major = register_chrdev(0, "key_async_drv", &key_async_drv_fo);

	key_async_class = class_create(THIS_MODULE, "key_async_drv");
	
	key_async_drv_class = class_device_create(key_async_class, NULL, MKDEV(major, 0), NULL, "buttons"); /* /dev/key_async_drv */

	pGPFCON = (unsigned int *)ioremap(0x56000050, 16);
	pGPFDAT = pGPFCON + 1;

	pGPGCON = (unsigned int *)ioremap(0X56000060, 16);
	pGPGDAT = pGPGCON + 1;

	return 0;

}

/* 卸载驱动 */
static void __exit key_async_drv_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	class_device_unregister(key_async_drv_class);
	class_destroy(key_async_class);
	unregister_chrdev(major, "key_async_drv");
	iounmap(pGPFCON);
	iounmap(pGPGCON);
}



/* 7. 其他完善：提供设备信息，自动创建设备节点                                     */

module_init(key_async_drv_init);
module_exit(key_async_drv_exit);

MODULE_LICENSE("GPL");


