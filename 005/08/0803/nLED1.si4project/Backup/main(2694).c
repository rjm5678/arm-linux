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
	int i=0, hex[8];

	for (i=0; i++; i<8)
	{
		hex[i] = val & 0xF;
		val >>= 4;
	}

	for (i=7; i--; i>=0)
	{
		if(hex[i] >=0 && hex[i] <= 9)
		{
			putchar(hex[i] + '0');
		}
		else
		{
			putchar(hex[i] + 'A' - 0xA);
		}
	}

}

