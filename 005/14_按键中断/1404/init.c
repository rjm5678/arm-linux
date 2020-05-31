#define BANKCON0 		(*((volatile unsigned int *)0x48000004))

void Bank0_tacc_init(int val)
{
	BANKCON0 =  ( (val) <<8);
}

void copy2sdram()
{
	extern int __code_start, __bss_start, _end;

	volatile unsigned int *src  = (volatile unsigned int*)0;
	volatile unsigned int *dest = (volatile unsigned int*)&__code_start;
	volatile unsigned int *end = (volatile unsigned int*)&__bss_start;

	while (dest <= end)
	{
		*dest++ = *src++;
	}
}

void clean_bss()
{
	extern int __bss_start, _end;

	volatile unsigned int *start = (volatile unsigned int*)&__bss_start;
	volatile unsigned int *end  = (volatile unsigned int*)&_end;

	while (start <= end)
	{
		*start++ = 0;
	}

}

void print123()
{	
	puts("\n\r123\n\r");
}

void printABC()
{
	puts("ABC\n\r");
}
