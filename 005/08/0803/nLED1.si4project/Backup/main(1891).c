void uart0_config();
int putchar(int c);
int getchar();
int puts(const char *s);
void Bank0_tacc_init(int val);
int led(void);
void sdram_init();
int sdram_test();
void delay(volatile d);
void print_hex(int val);


char g_char = 'A';
char g_char2 = 'a';
int g_val = 0xABC;

int main()
{


	uart0_config();	//配置UART

	puts("\n\rg_val = ");
	print_hex(g_val);

	while(1)
	{
		putchar(g_char++);
		putchar(g_char2++);
		delay(1000000);
	}
	
	return 0;
		
}

void print_hex(int val)
{
	int i=0, hex=0, mask = 0xF;

	for (i=7; i--; i>=0)
	{
		hex = val & mask;

		if(hex >=0 && hex <= 9)
		{
			putchar(hex + '0');
		}
		else
		{
			putchar(hex + 'A' - 0xA);
		}
		
		mask <<= 4;
	}

}
