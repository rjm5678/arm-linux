void delay(volatile d)
{
	while(d--);
}


int led(void)
{
	unsigned int *pGPFCON = (unsigned int *)0X56000050;
	unsigned int *pGPFDAT = (unsigned int *)0X56000054;
	int val = 0;
	*pGPFCON &= ~((3<<8)| (3<<10) | (3<<12)); /* 把GPFCON 8~13位清零 */
	*pGPFCON |=  ((1<<8)| (1<<10) | (1<<12)); /* 把GPFCON 9、11、13位置1，即设置为输出模式*/
	/*
	while(1)
	{
		*pGPFDAT &= ~(7<<4); /* 清零 
		*pGPFDAT |= (val<<4);
		delay(100000);
		val++;
		if (val ==8)
			val =0;
	}
	*/
	
	val = 4;
	
	while(1)
	{
		*pGPFDAT |= (7<<4);	/* 把GPFDAT 4~6置1，即熄灭LED */
		*pGPFDAT &= ~(1<<val); /* 循环点亮 */
		delay(100000);
		val++;
		if(val==7) val=4;
	} 
}	

