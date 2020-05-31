void uart0_config();
int putchar(int c);
int getchar();
int puts(const char *s);
void Bank0_tacc_init(int val);
int led(void);
void sdram_init();
int sdram_test();
void delay(volatile d);
unsigned char * hex2chars(int val);


char g_char = 'A';
char g_char2 = 'a';
int g_val = 5;

int main()
{


	uart0_config();	//配置UART

	puts("\n\rg_val = ");
	puts(hex2chars(g_val));

	while(1)
	{
		putchar(g_char++);
		putchar(g_char2++);
		delay(1000000);
	}
	
	return 0;
		
}

unsigned char * hex2chars(int val)
{
	int i=0, hex=0;
	unsigned char * chars[8] = {0};

	for (i=7; i--; i>=0)
	{
		hex = val & 0xF;

		if(hex >=0 && hex <= 9)
		{
			chars[i] = hex + '0';
		}
		else
		{
			chars[i] = hex + 'A' - 0xA;
		}
		
		val <<= 4;
	}

	return chars;
}

