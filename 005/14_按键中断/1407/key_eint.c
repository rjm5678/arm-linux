#include "s3c2440_soc.h"



/* 按键处理函数，包括按键处理
irq - 0 ： EINT0 （S2 - EINT0）
irq - 2 ： EINT2 （S3 ~ EINT2）
irq - 5 ： EINT8_23 （S4 ~ EINT11，S5 ~ EINT19）

*/
int key_action(int irq)
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



void handle_irq_c()
{
	/* 分辨中断源 */
	/*INTOFFSET 寄存器** Indicate the IRQ interrupt request source
	 - 地址：0x4A000014
   	 只读寄存器
	 - 0 ： EINT0 （S2 - EINT0）
 	 - 2 ： EINT2 （S3 ~ EINT2）
 	 - 5 ： EINT8_23 （S4 ~ EINT11，S5 ~ EINT19）
 	 */
	int bit = INTOFFSET;	//读出来是哪个中断


	/* 处理中断 */
	if(bit == 0 || bit == 2 || bit == 5)
		key_action(bit);

	/* 清除中断 */
	SRCPND = (1<<bit);
	INTPND = (1<<bit);	
}



