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
	int val =0;
	
//	key_eint_init();
	timer_init();
	led_init();
	nand_init();
	nand_flash_test();
	nor_flash_test();


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






