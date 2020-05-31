#include "s3c2440_soc.h"


void Bank0_tacc_init(int val)
{
	BANKCON0 =  ( (val) <<8);
}

/* 代码重定位 */
void copy2sdram()
{
	extern int __code_start, __bss_start, _end;

	volatile unsigned int *src  = (volatile unsigned int*)0;
	volatile unsigned int *dest = (volatile unsigned int*)&__code_start;
	volatile unsigned int *end = (volatile unsigned int*)&__bss_start;

	while (dest <= end)
	{
		*dest++ = *src++;
	}
}

/* bss段清零 */
void clean_bss()
{
	extern int __bss_start, _end;

	volatile unsigned int *start = (volatile unsigned int*)&__bss_start;
	volatile unsigned int *end  = (volatile unsigned int*)&_end;

	while (start <= end)
	{
		*start++ = 0;
	}

}

void led_init()
{
	unsigned int *pGPFCON = (unsigned int *)0X56000050;
	unsigned int *pGPFDAT = (unsigned int *)0X56000054;
	
	*pGPFCON &= ~((3<<8)| (3<<10) | (3<<12)); /* 把GPFCON 8~13位清零 */
	*pGPFCON |=  ((1<<8)| (1<<10) | (1<<12)); /* 把GPFCON 9、11、13位置1，即设置为输出模式*/
}


/* 初始化中断控制器 */
void interrput_init()
{
	/* SRCPND寄存器 Indicate the interrupt request status.
	 * 一共32位代表不同的中断，置0是关闭中断，置1是使能中断
	 - 地址： 0X4A000000
	 - [0] ： EINT0 = 0x1（S2 - EINT0）
	 - [2] ： EINT2 = 0x1（S3 ~ EINT2）
	 - [5] ： EINT8_23 = 0x1（S4 ~ EINT11，S5 ~ EINT19）
	 
	 * INTMSK 寄存器  中断屏蔽寄存器
	 * 置1是屏蔽中断源，置0是使能，默认是全部屏蔽，所以需要设置。
	 - 地址：0X4A000008
	 - [0] ： EINT0 = 0x0（S2 - EINT0）
	 - [2] ： EINT2 = 0x0（S3 ~ EINT2）
	 - [5] ： EINT8_23 = 0x0（S4 ~ EINT11，S5 ~ EINT19）
	 -  即 [0X4A000008] &= ((0<<0) | (0<<2) | (0<<5))
	 */

	 INTMSK &= ~((1<<0) | (1<<2) | (1<<5));
	 
}




/* 初始化按键中断 */
void key_eint_init()
{
	/* 初始化按键寄存器
	 * 设置GPFCON的GPF0/2、GPG3/11为中断引脚 
	 *  按键寄存器 
	 * C_S2 ~ 0  ~ GPF0 
	 * C_S3 ~ 4  ~ GPF2 
	 * C_S4 ~ 6  ~ GPG3 
	 * C_S5 ~ 22 ~ GPG11 
	 * D_S2 ~ 0  ~ GPF0 
	 * D_S3 ~ 2  ~ GPF2 
	 * D_S4 ~ 3  ~ GPG3 
	 * D_S5 ~ 11 ~ GPG11 
	 */
	GPFCON &= ~((3<<C_S2) | (3<<C_S3)); 	/* 把GPFCON 需要位置清零 */
	GPGCON &= ~((3<<C_S4) | (3<<C_S5)); 	/* 把GPGCON 需要位置清零 */

	
	GPFCON |=  ((2<<C_S2) | (2<<C_S3)); 	/* 把GPFCON 需要位置置位0x10 */
	GPGCON |=  ((2<<C_S4) | (2<<C_S5)); 	/* 把GPGCON 需要位置置位0x10 */

	/* EXTINT0 寄存器 External interrupt control register 0
	 - 地址：0x56000088
	 - [2:0]  ：EINT0 = 0x111（11x = Both edge triggered）
	 - [10:8] ：EINT2 = 0x111（11x = Both edge triggered）

	 * EXTINT1 寄存器 External interrupt control register 1
	 * 地址：0x5600008c
	 - [14:12] ：EINT11 = 0x111（11x = Both edge triggered）

	 * EXTINT2 寄存器 External interrupt control register 2
	 * 地址：0x56000090
	 - [14:12] ：EINT19 = 0x111（11x = Both edge triggered）
	 
	 * EINTMASK 寄存器 I/O中断屏蔽寄存器，置1是屏蔽中断源，置0是使能，默认是全部屏蔽，所以需要设置。
	 - 地址：0x560000a4（External interrupt mask register ） 
	 - [11] ：EINT11 = 0x0
	 - [19] ：EINT19 = 0x0
	 */
	EXTINT0 |= ((7<<0) | (7<<8)); 	/* s2, s3*/
	EXTINT1 |= ((7<<12)); 			/* s4 */
	EXTINT2 |= ((7<<12));			/* s5 */

	EINTMASK &= ~((1<<11) | (1<<19));	/* 使能外部中断，关闭屏蔽信号 */

}

