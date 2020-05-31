#include "s3c2440_soc.h"

char g_char = 'A';
char g_char2 = 'a';
int g_val = 0;

void delay(volatile d)
{
	while(d--);
}


int main()
{
	key_eint_init();
	timer_init();
	led_init();
//	uart0_init();	//配置UART

	puts("\n\rg_val = ");
	print_hex(g_val);
	puts("\n\r");
	while(1)
	{
		putchar(g_char++);
		putchar(g_char2++);
		delay(1000000);
	}
	
	return 0;
		
}






