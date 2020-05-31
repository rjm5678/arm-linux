#include "s3c2440_soc.h"

int putchar(int c)
{
	/* UTRSTAT0寄存器
	 - [2] Transmitter empty 数据发完了就是1
	 - [1] Transmit buffer empty 数据缓存空的就是1
	 - [0] Receive buffer data ready 有数据可接收时是1
	 */
	while(!(UTRSTAT0&(1<<2))); //当寄存器第二位是1时一直循环
	UTXH0 = (unsigned char)c;	//UTXH0是1个字节的

}

int getchar()
{
	/* UTRSTAT0寄存器
	 - [2] Transmitter empty 数据发完了就是1
	 - [1] Transmit buffer empty 数据缓存空的就是1
	 - [0] Receive buffer data ready 有数据可接收时是1
	 */
	while(!(UTRSTAT0&(1<<0))); 
	return URXH0;
}

int puts(const char *s)
{
	while(*s)
	{
		putchar(*s++);		
	}
}
void print_hex(int val)
{
	int i=0;
	unsigned char hex[8];

	for (i=0; i<8; i++)
	{
		hex[i] = val & 0xF;
		val >>= 4;
	}
	puts("0x");
	for (i=7; i>=0; i--)
	{
		if(hex[i] >=0 && hex[i] <= 9)
		{
			putchar(hex[i] + '0');
		}
		else if (hex[i] >= 0xA && hex[i] <= 0xF)
		{
			putchar(hex[i] + 'A' - 0xA);
		}
		else putchar('?');
	}

}
void print123()
{	
	puts("\n\r123");
}

void printABC()
{
	puts("\n\rABC");
}


