#include "s3c2440_soc.h"

typedef void(*irq_func)(int);
irq_func irq_array[32];

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
	printf("bit = %d, INTMSK = 0x%x, INTSUBMSK = 0x%x\n\r", bit, INTMSK, INTSUBMSK);
	printf("SUBSRCPND = 0x%x, SRCPND = 0x%x, INTPND = 0x%x\n\r", SUBSRCPND, SRCPND, INTPND);


	/* 处理中断 */
	irq_array[bit](bit);

	/* 清除中断 */
	SRCPND = (1<<bit);
	INTPND = (1<<bit);	
}

/* 注册中断的指针函数, 附加使能总中断
 * irq - 第几个处理函数，fp - 传入的中断处理函数
 */
void register_irq(int irq, irq_func fp)
{
	irq_array[irq] = fp;

	/* [INTMSK] 寄存器  中断屏蔽寄存器
	 * 置1是屏蔽中断源，置0是使能，默认是全部屏蔽，所以需要设置。
	 - 地址：0X4A000008
	 - [0] ： EINT0 = 0x0（S2 - EINT0）
	 - [2] ： EINT2 = 0x0（S3 ~ EINT2）
	 - [5] ： EINT8_23 = 0x0（S4 ~ EINT11，S5 ~ EINT19）
	 -  即 [0X4A000008] &= ((0<<0) | (0<<2) | (0<<5))
	 */
	INTMSK &= ~(1<<irq);
	printf("INTMSK = 0x%x, INTSUBMSK = 0x%x\n\r", INTMSK, INTSUBMSK);
	printf("SUBSRCPND = 0x%x, SRCPND = 0x%x, INTPND = 0x%x\n\r", SUBSRCPND, SRCPND, INTPND);
}

