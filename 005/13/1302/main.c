void uart0_config();
int putchar(int c);
int getchar();
int puts(const char *s);
void Bank0_tacc_init(int val);
int led(void);
void sdram_init();
int sdram_test();
void delay(volatile d);


char g_char = 'A';
char g_char2 = 'a';

int main()
{


	uart0_config();	//配置UART

	puts("\nhello ! \n\r");

	while(1)
	{
		putchar(g_char++);
		putchar(g_char2++);
		delay(1000000);
	}
	


	return 0;
		
}

