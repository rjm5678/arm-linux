void uart0_config();
int putchar(int c);
int getchar();
int puts(const char *s);
void Bank0_tacc_init(int val);
int led(void);



int main()
{
	unsigned char c;

	uart0_config();	//配置UART

	puts("Hello!!!\n\r");
	puts("Enter a val: ");

	while (1)
	{
		
		c = getchar();
		putchar(c);
		
		Bank0_tacc_init(c-'0');
		led();
		


		
	}

	return 0;
		
}

