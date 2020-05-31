void delay(volatile d)
{
	while(d--);
}

#define GPFCON (*((volatile unsigned int *)0X56000050))
#define GPFDAT (*((volatile unsigned int *)0X56000054))

#define GPGCON (*((volatile unsigned int *)0X56000060))
#define GPGDAT (*((volatile unsigned int *)0X56000064))

/* LED寄存器 */
#define C_LED1 8  //GPF4
#define C_LED2 10 //GPF5
#define C_LED3 12 //GPF6
#define D_LED1 4  //GPF4
#define D_LED2 5  //GPF5
#define D_LED3 6  //GPF6

/* 按键寄存器 */
#define C_S2 0 /* GPF0 */
#define C_S3 4 /* GPF2 */
#define C_S4 6 /* GPG3 */
#define D_S2 0 /* GPF0 */
#define D_S3 2 /* GPF2 */
#define D_S4 3 /* GPG3 */
int main(void)
{

	/* 设置GPFCON的GPF4/5/6为输出引脚 
	 * GPF4(8/9位) GPF5(10/11位) GPF6(12/13位)
	 */
	GPFCON &= ~((3<<C_LED1)| (3<<C_LED2) | (3<<C_LED3)); /* 把GPFCON 需要位置清零 */
	GPFCON |=  ((1<<C_LED1)| (1<<C_LED2) | (1<<C_LED3)); /* 把GPFCON 需要位置1，即设置为输出模式*/
	
	/* 设置GPFCON的GPF0/2、GPG3为输入引脚 
	 * GPF0(0/1位) GPF2(4/5位)
	 */
	GPFCON &= ~((3<<C_S2) | (3<<C_S3)); /* 把GPGCON 需要位置清零 */
	GPGCON &= ~(3<<C_S4); /* 把GPGCON 6~7 位清零 */


	/* S2-GPF0 、 S3-GPF2 、 S4 - GPG3
	 * S2控制GPF6、S3控制GPF5、S4控制GPF4
	 * 如果按键对应DAT寄存器是零，即按键按下，反之
	 */
	while(1)
	{
		
		/* S2控制LED3 */
		if (GPFDAT&(1<<D_S2)){
			GPFDAT |= (1<<D_LED3);
		}
		else{
			GPFDAT &= ~(1<<D_LED3);
		}
		
		/* S3控制LED2 */
		if (GPFDAT&(1<<D_S3)){
			GPFDAT |= (1<<D_LED2);
		}
		else{
			GPFDAT &= ~(1<<D_LED2);
		}
		
		/* S4控制LED1 */
		if (GPGDAT&(1<<D_S4)){
			GPFDAT |= (1<<D_LED1);
		}
		else{
			GPFDAT &= ~(1<<D_LED1);
		}
	} 
}	

