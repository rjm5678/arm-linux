#include "s3c2440_soc.h"

#define TIMER_NUM  32
#define NULL  ((void *)0)


typedef void(*timer_func)(void);

typedef struct timer_desc
{
	char *name;
	timer_func fp;
}timer_desc, *p_timer_desc;

timer_desc timer_array[TIMER_NUM];



/* 注册中断的指针函数, 附加使能总中断
 * name - 函数的名字，fp - 传入的中断处理函数
 */
int register_timer(char *name, timer_func fp)
{
	int i;

	for (i=0; i<TIMER_NUM; i++)
	{
		if (!timer_array[i].fp)
		{
			timer_array[i].fp = fp;
			timer_array[i].name = name;
			return 0;
		}

		
	}

	return -1;
}


int unregister_timer(char *name)
{
	int i;

	for (i=0; i<TIMER_NUM; i++)
	{
		if (!strcmp(timer_array[i].name, name))
		{
			timer_array[i].fp = NULL;
			timer_array[i].name = NULL;

		}

		return 0;
	}

	return -1;
}
void timer_irq()
{
	int i=0;
	
	for (i=0; i<TIMER_NUM; i++)
	{
		if (timer_array[i].fp)
			timer_array[i].fp();
	}
	
}
void timer_init()
{

	/*
	[TCFG0 寄存器] 设置预分频
	 - 地址：0x51000000
	 - [7:0] = 99 = 0x63 ：Prescaler 0

	[TCFG1 寄存器] 设置分频
	 - 地址：0x51000004
	 - [3:0] = 0x0011 ：MUX 0 （ 设置为timer 0 为1/16分频）

	[TCON 寄存器] 控制寄存器
	 - 地址：0x51000008
	 - [0] = 0x1 ：Timer 0 start/stop  （ Start for Timer 0）
	 - [1] = 0x1 ：Timer 0 manual update [^note]（Update TCNTB0 & TCMPB0）
	 - [3] = 0x1 ：Timer 0 auto reload on/off （auto reload）
	[^note]: The bit has to be cleared at next writing.

	[TCNTB0 寄存器] Timer 0 计数缓冲寄存器
	- 地址：0x5100000C
	- [15:0]  = 15625
	*/

		/* Timer clk = PCLK / {prescaler value+1} / {divider value} 
	             = 50000000/(49+1)/16
	             = 62500
	             = 10ms*/
	TCFG0  =   49;
	TCFG1 &=  (0xF);
	TCFG1 |=  3;
	TCNTB0 =  625;

	TCON  |=  (1<<1);
	
	TCON  &= ~(1<<1);	//The bit has to be cleared at next writing.
	TCON  |=  ((1<<0) | (1<<3));

	register_irq(10, timer_irq);
}


