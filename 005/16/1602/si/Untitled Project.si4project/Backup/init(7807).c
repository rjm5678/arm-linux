#include "s3c2440_soc.h"


void Bank0_tacc_init(int val)
{
	BANKCON0 =  ( (val) <<8);
}

/* 在零地址写数据
 * 写成功了返回0，是Nand启动
 * 不成功的返回1，是Nor 启动
 */
int is_boot_from_nand()
{
	volatile unsigned int *p = (volatile unsigned int *)0;
	unsigned int val = *p;	//保存原来的值
			
	*p = 0x12345678;
	if (*p == 0x12345678)
	{
		*p = val;			//恢复原来的值
		return 0;			//Nand 启动
	}
	else
	{
		return 1;				//Nor 启动
	}

}

/* 代码重定位 */
void copy2sdram()
{
	extern int __code_start, __bss_start, _end;

	volatile unsigned int *src  = (volatile unsigned int*)0;
	volatile unsigned int *dest = (volatile unsigned int*)&__code_start;
	volatile unsigned int *end = (volatile unsigned int*)&__bss_start;
	unsigned int len = (int)&__bss_start - (int)&__code_start;

	

	if (is_boot_from_nand())	
	{
		while (dest <= end)			//Nor 启动
		{
			*dest++ = *src++;
		}
	}
	else
	{
		nand_init();
		nand_read(src, dest, len);
	}
	
}

/* bss段清零 */
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



