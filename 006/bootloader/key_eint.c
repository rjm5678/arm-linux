#include "s3c2440_soc.h"


/* 按键处理函数，包括按键处理
irq - 0 ： EINT0 （S2 - EINT0）
irq - 2 ： EINT2 （S3 ~ EINT2）
irq - 5 ： EINT8_23 （S4 ~ EINT11，S5 ~ EINT19）

*/
int key_irq(int irq)
{
	unsigned int val = EINTPEND;


	/* S2-GPF0 、 S3-GPF2 、 S4 - GPG3
	 * S2~GPF6、S3~GPF5、S4~GPF4
	 * 如果按键对应DAT寄存器是零，即按键按下，反之 
	 */

	/* S2控制LED3 */
	if (irq == 0)
	{
		if (GPFDAT&(1<<D_S2)){
			GPFDAT |=  (1<<D_LED3);
		}
		else{
			GPFDAT &= ~(1<<D_LED3);
		}
	}
	
	
	/* S3控制LED2 */
	else if (irq == 2)
	{
		if (GPFDAT&(1<<D_S3)){
			GPFDAT |=  (1<<D_LED2);
		}
		else{
			GPFDAT &= ~(1<<D_LED2);
		}
	}

	
	/* S4控制LED1 */
	else if (irq == 5)
	{
		if (val & (1<<11))
		{
			if (GPGDAT&(1<<D_S4)){
				GPFDAT |=  (1<<D_LED1);
			}
			else{
				GPFDAT &= ~(1<<D_LED1);
			} 
		}
		
		else if (val & (1<<19))
		{
			if (GPGDAT&(1<<D_S5)){
				GPFDAT |=  ((1<<D_LED1) | (1<<D_LED2) | (1<<D_LED3));
			}
			else{
				GPFDAT &= ~((1<<D_LED1) | (1<<D_LED2) | (1<<D_LED3));
			} 	
		}
		

	}
	EINTPEND = val;			//清除外部中断
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

	register_irq(0, key_irq);
	register_irq(2, key_irq);
	register_irq(5, key_irq);
}


