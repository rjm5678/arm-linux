

int main()
{
	unsigned int *pGPFCON = (unsigned int *)0X56000050;
	unsigned int *pGPFDAT = (unsigned int *)0X56000054;

	*pGPFCON = 0x0100; /*设置GPF4为输出引脚*/
	*pGPFDAT = 0X0000;	/*设置GPF4输出0*/
	
	return 0;
}
