#include "../include/s3c2440_soc.h"

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

void enter_wait_pen_down_mode(void)
{
	ADCTSC = WAIT_PEN_DOWN | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE | XP_DISABLE | XM_DISABLE | WAIT_INT_MODE;
}

void enter_wait_pen_up_mode(void)
{
	ADCTSC = WAIT_PEN_UP | PULLUP_ENABLE | YM_ENABLE | YP_DISABLE | XP_DISABLE | XM_DISABLE | WAIT_INT_MODE;
}


/* 这个函数处理中断 */
void touchscreen_irq_handle()
{
	/* 如果是触摸屏松开状态 */
	if (ADCDAT0 & (1<<15))
	{
		printf("pen up\n\r");
		enter_wait_pen_down_mode();	
	}

	/* 如果触摸屏是按下状态 */
	
	else
	{
		printf("pen down\n\r");
		enter_wait_pen_up_mode();
	}
	SUBSRCPND = ((1<<ADC_INT_BIT) | (1<<TC_INT_BIT));
}

/* 设置寄存器,就是ADC寄存器 */
void tc_reg_init()
{
	/* [15] : ECFLG,  1 = End of A/D conversion
	 * [14] : PRSCEN, 1 = A/D converter prescaler enable
	 * [13:6]: PRSCVL, adc clk = PCLK / (PRSCVL + 1)
	 * [5:3] : SEL_MUX, 000 = AIN 0
	 * [2]   : STDBM
	 * [0]   : 1 = A/D conversion starts and this bit is cleared after the startup.
	 */
	ADCCON = (1<<14) | (49<<6) | (0<<3);

	ADCDLY = 0xff;		
}

/* 注册中断 */
void tc_eint_init()
{
	register_irq(INT_ADC_TC, touchscreen_irq_handle);

	/* 取消sub中断屏蔽信号 */
	INTSUBMSK &= ~((1<<ADC_INT_BIT) | (1<<TC_INT_BIT));

}
/* 触摸屏中断寄存器初始化，触摸中断初始化 */
void touchscreen_init()
{
	/* 触摸屏中断寄存器初始化 */	
	tc_reg_init();

	/* 注册触摸中断, 使能中断啥的 */
	tc_eint_init();

	/* 进入中断等待状态 */
	enter_wait_pen_down_mode();
}
