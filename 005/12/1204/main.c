void uart0_config();
int putchar(int c);
int getchar();
int puts(const char *s);
void Bank0_tacc_init(int val);
int led(void);
void sdram_init();
int sdram_test();



int main()
{
	unsigned char c;
	volatile unsigned int *p = (volatile unsigned int*)0x30000000;
//	uart0_config();	//配置UART
//	sdram_init();


		if(sdram_test() == 0)
			{led();}		


	return 0;
		
}

