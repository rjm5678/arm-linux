int led_on(int which)
{
	unsigned int *pGPFCON = (unsigned int *)0X56000050;
	unsigned int *pGPFDAT = (unsigned int *)0X56000054;
	
	if (which == 4) /*如果是GPF4*/
	{
		*pGPFCON = 0x0100; /*设置GPF4为输出引脚*/
		
	}
	
	else if (which == 5)/*如果是GPF5*/
	{
		*pGPFCON = 0x0400; /*设置GPF5为输出引脚*/
		
	}
	*pGPFDAT = 0X0000;	/*设置GPF4 or 5输出0*/
}	

void delay(volatile d)
{
	while(d--);
}

int main()
{
	unsigned int *pGPFCON = (unsigned int *)0X56000050;
	unsigned int *pGPFDAT = (unsigned int *)0X56000054;

	*pGPFCON = 0x0100; /*设置GPF4为输出引脚*/
	*pGPFDAT = 0X0000;	/*设置GPF4输出0*/
	
	return 0;
}
