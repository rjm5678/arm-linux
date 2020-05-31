#include "s3c2440_soc.h"




void timer_irq()
{
#if 0
	static int val = 4;
	
	GPFDAT |= (7<<4);	/* 把GPFDAT 4~6置1，即熄灭LED */
	GPFDAT &= ~(1<<val); /* 循环点亮 */

	val++;
	if(val==7) val=4;
#endif
}

/* 初始化定时器 */
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
	TCFG0  =   99;
	TCFG1 &=  (0xF);
	TCFG1 |=  3;
	TCNTB0 =  15625;

	TCON  |=  (1<<1);
	
	TCON  &= ~(1<<1);	//The bit has to be cleared at next writing.
	TCON  |=  ((1<<0) | (1<<3));

	register_irq(10, timer_irq);
}


