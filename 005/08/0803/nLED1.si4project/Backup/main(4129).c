void uart0_config();
int putchar(int c);
int getchar();
int puts(const char *s);


int main()
{
	unsigned char c;

	uart0_config();	//配置UART

	puts("Hello!!!\n\r");

	while (1)
	{
		c = getchar();
	
		if(c == '\r') putchar('\n');
		if(c == '\n') putchar('\r');

		putchar(c);
	}

	return 0;
		
}
