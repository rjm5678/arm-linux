#include "s3c2440_soc.h"

void led_timer_irq()
{
	static int val = 4;
	static int cnt = 0;

	if(cnt++ != 50)
		return ;
	
	GPFDAT |= (7<<4);	/* 把GPFDAT 4~6置1，即熄灭LED */
	GPFDAT &= ~(1<<val); /* 循环点亮 */

	val++;
	if(val==7) val=4;

	cnt = 0;
	
}

void led_init()
{
	
	GPFCON &= ~((3<<8)| (3<<10) | (3<<12)); /* 把GPFCON 8~13位清零 */
	GPFCON |=  ((1<<8)| (1<<10) | (1<<12)); /* 把GPFCON 9、11、13位置1，即设置为输出模式*/

	register_timer("led", led_timer_irq);
}



int led_flow(void)
{
	int val = 4;
	
	while(1)
	{
		GPFDAT |= (7<<4);	/* 把GPFDAT 4~6置1，即熄灭LED */
		GPFDAT &= ~(1<<val); /* 循环点亮 */
		delay(100000);
		val++;
		if(val==7) val=4;
	} 
}	

