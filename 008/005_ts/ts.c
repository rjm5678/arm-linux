#include <asm-arm/arch-s3c2410/regs-adc.h>
#include <asm-arm/arch-s3c2410/regs-gpio.h>
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
#include <linux/input.h>

/* ADC和触摸屏 */
volatile unsigned int *ADCCON 	= NULL;	
volatile unsigned int *ADCTSC 	= NULL;	
volatile unsigned int *ADCDLY 	= NULL;	
volatile unsigned int *ADCDAT0 = NULL;	
volatile unsigned int *ADCDAT1 = NULL;	
volatile unsigned int *ADCUPDN = NULL;		


/* ADCTSC's bits */
#define ADC_INT_BIT (10)
#define TC_INT_BIT  (9)

#define INT_ADC_TC   (31)

/* ADCTSC 寄存器0~8 */
/* UD_SEN */
#define WAIT_PEN_DOWN    (0<<8)
#define WAIT_PEN_UP      (1<<8)
/* YM_SEN */
#define YM_ENABLE        (1<<7)
#define YM_DISABLE       (0<<7)
/* YP_SEN */
#define YP_ENABLE        (0<<6)
#define YP_DISABLE       (1<<6)
/* XM_SEN */
#define XM_ENABLE        (1<<5)
#define XM_DISABLE       (0<<5)
/* XP_SEN */
#define XP_ENABLE        (0<<4)
#define XP_DISABLE       (1<<4)
/* PULL_UP */
#define PULLUP_ENABLE    (0<<3)
#define PULLUP_DISABLE   (1<<3)
/* AUTO_PST */
#define AUTO_PST         (1<<2)
/* XY_PST */
#define WAIT_INT_MODE    (3)
#define NO_OPR_MODE      (0)

static struct input_dev *ts_dev;
static struct timer_list ts_timer;
static volatile int g_ts_timer_enable = 0;

void enter_wait_pen_down_mode(void)
{
	*ADCTSC = WAIT_PEN_DOWN | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE | XP_DISABLE | XM_DISABLE | WAIT_INT_MODE;
}

void enter_wait_pen_up_mode(void)
{
	*ADCTSC = WAIT_PEN_UP | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE | XP_DISABLE | XM_DISABLE | WAIT_INT_MODE;
}
void enter_auto_measure_mode(void)
{
	
	*ADCTSC = AUTO_PST | NO_OPR_MODE;
}
void enable_adc_timer(void)
{
	g_ts_timer_enable = 0;
}
void disable_adc_timer(void)
{
	g_ts_timer_enable = 1;
}

int get_adc_timer_status(void)
{
	mod_timer(&ts_timer, jiffies+1);
	return g_ts_timer_enable;
}



/* 每10ms启动一次 */
void touchscreen_timer_irq(int irq, void *dev_id)
{
	if (get_adc_timer_status() == 1)
	{
		return ;
	}
	if ((*ADCDAT0 & (1<<15)))	//如果触摸屏没有被按下，返回
	{
		disable_adc_timer();
		enter_wait_pen_down_mode();
		return ;
	}
		

	else
	{
		enter_auto_measure_mode();
		*ADCCON |= (1<<0);
	}

	

}

void Isr_Ts(void)	
{
/* 如果是触摸屏松开状态 */
	if (*ADCDAT0 & (1<<15))
	{
		printk("pen up\n\r");
		enter_wait_pen_down_mode();	
	}

	/* 如果触摸屏是按下状态 */
	
	else
	{
		printk("pen down\n\r");
		//enter_wait_pen_up_mode();
		enter_auto_measure_mode();

		/* 启动ADC */
		*ADCCON |= (1<<0);
	}
	
	
}

void Isr_adc(void)
{
	int x,y;

	x = *ADCDAT0;
	y = *ADCDAT1;

	if (!(x & (1<<15))) 	//如果仍然在按下
	{
		
		x &= (0x3ff);
		y &= (0x3ff);
		printk("x = %d, y = %d\n\r",x, y);

		enable_adc_timer();
	}
	else
	{
		disable_adc_timer();
		enter_wait_pen_down_mode();
	}

	enter_wait_pen_up_mode();
}

/* 这个函数处理中断 */
int AdcTsIntHandle(int irq, void *pd)
{
	printk("touching: ");
//	if (SUBSRCPND & (1<<TC_INT_BIT))
		Isr_Ts();
//	if (SUBSRCPND & (1<<ADC_INT_BIT))
//		Isr_adc();
	
//	SUBSRCPND = ((1<<ADC_INT_BIT) | (1<<TC_INT_BIT));

	return IRQ_RETVAL(IRQ_HANDLED);
}

/* 设置寄存器,就是ADC寄存器 */
void adc_reg_init(void)
{
	/* [15] : ECFLG,  1 = End of A/D conversion
	 * [14] : PRSCEN, 1 = A/D converter prescaler enable
	 * [13:6]: PRSCVL, adc clk = PCLK / (PRSCVL + 1)
	 * [5:3] : SEL_MUX, 000 = AIN 0
	 * [2]   : STDBM
	 * [0]   : 1 = A/D conversion starts and this bit is cleared after the startup.
	 */
	ADCCON = (unsigned int *)ioremap(0x58000000, 16);
	ADCTSC = ADCCON + 1;
	ADCDLY = ADCTSC + 1;
	ADCDAT0 = ADCDLY + 1;
	ADCDAT1 = ADCDAT0 + 1;
	ADCUPDN = ADCDAT1 + 1;

	
	*ADCCON = (1<<14) | (49<<6) | (0<<3);

	*ADCDLY = 60000;		
}

/* 触摸屏中断寄存器初始化，触摸中断初始化 */
static int touchscreen_init(void)
{
	int  ret;


	/* 1. 分配一个input_dev结构体 */
	ts_dev = input_allocate_device();


	/* 2. 设置 */
	/* 2.1 能产生哪类事件 */
	set_bit(EV_KEY, ts_dev->evbit);
	set_bit(EV_ABS, ts_dev->evbit);

	/* 2.2 能产生这类操作里的哪些事件: L,S,ENTER,LEFTSHIT */
	set_bit(BTN_TOUCH, ts_dev->keybit);

	input_set_abs_params(ts_dev, ABS_X, 0, 0x3FF, 0, 0);
	input_set_abs_params(ts_dev, ABS_Y, 0, 0x3FF, 0, 0);
	input_set_abs_params(ts_dev, ABS_PRESSURE, 0, 1, 0, 0);

	/* 3. 注册 */
	ret = input_register_device(ts_dev);

	
	/* 4. 硬件相关的操作 */
	/* 触摸屏中断寄存器初始化 */	
	adc_reg_init();

	/* 注册触摸中断 */
	ret = request_irq(IRQ_ADCPARENT, AdcTsIntHandle, IRQF_SAMPLE_RANDOM, "ts_dev", NULL);

	/* 注册定时器中断函数 */
//	register_timer("touchscreen", touchscreen_timer_irq);
/*
	init_timer(&ts_timer);
	ts_timer.function = touchscreen_timer_irq;
	add_timer();
	mod_timer(&ts_timer, jiffies+1);
*/
	/* 进入中断等待状态 */
	enter_wait_pen_down_mode();

	return 0;
}
static void touchscreen_exit(void)
{
	iounmap(ADCCON); 
	iounmap(ADCTSC);
	iounmap(ADCDLY);
	iounmap(ADCDAT0);
	iounmap(ADCDAT1);
	iounmap(ADCUPDN);

	free_irq(IRQ_ADCPARENT, NULL);
	input_free_device(ts_dev);

}

module_init(touchscreen_init);

module_exit(touchscreen_exit);

MODULE_LICENSE("GPL");

