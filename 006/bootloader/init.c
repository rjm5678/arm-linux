#include "s3c2440_soc.h"
#include "string_utils.h"

void nand_init()
{
	/*
	**NFCONF** NAND flash configuration register 0x4E000000
	 - [13:12] = 0 ： Duration = 0 = HCLK x TACLS =10ns x 0
	 - [10:8] = 1 ：Duration = 12 ≤ 20 = HCLK x ( TWRPH0 + 1 ) = 10ns x (1+1) 
	 - [6:4] = 0 ：Duration = 5 ≤ 10 = HCLK x ( TWRPH1 + 1 ) = 10ns x (0+1) 
	**NFCONT** NAND flash control register 0x4E000004
	- [0] = 1 ：NAND flash controller enable
	- [1] = 1 ：Reg_nCE 片选信号（初始化先禁止片选，要用才片选）
	- [4] = 1 ：InitECC （Initialize ECC decoder/encoder(Write-only)
	 */

	NFCONF = ((0<<12) | (1<<8) | (0<<4));		//设置时序
	NFCONT = (( 1<<0) | (1<<1) | (1<<4));		//初始化
}

void nand_wait_ready()
{
	/*
	**NFSTAT** 状态寄存器 0x4E000020
	- [0] 0: NAND Flash memory busy
	- [0] 1: NAND Flash memory ready to operate
	 */

	while(!(NFSTAT & 1));		//当[0]不等于1一直等待
}
void nand_select()
{
	NFCONT &=  ~(1<<1);
}
void nand_deselect()
{
	NFCONT |=   (1<<1);
}
unsigned char nand_data()
{
	return	NFDATA;
}
void nand_w_data(unsigned char val)
{
	NFDATA = val;
}

void nand_cmd(unsigned char cmd)	//8位的寄存器
{
	int i = 0;
	NFCCMD = cmd;
	for(i=0; i<10; i++); 			//延时一会保证命令完成
}
void nand_addr_byte(unsigned char addr)			//8位的寄存器
{
	int i = 0;
	NFADDR = addr;
	for(i=0; i<10; i++); 			//延时一会保证命令完成
}

void read_id()
{
	unsigned char buf[5] = {0};

	nand_select();
	nand_cmd(0x90);				
	nand_addr_byte(0X00);		//读id的命令

	buf[0] = nand_data();
	buf[1] = nand_data();
	buf[2] = nand_data();
	buf[3] = nand_data();
	buf[4] = nand_data();

	puts("\n\r");
	puts("maker   id  =");print_hex(buf[0]);puts("\n\r");
	puts("device  id  =");print_hex(buf[1]);puts("\n\r");
 	puts("3th     id  =");print_hex(buf[2]);puts("\n\r");
 	puts("4th     id  =");print_hex(buf[3]);puts("\n\r");
	puts("Page Size   =");print_hex(1<<(buf[3] & 0x3));puts("\n\r");
	puts("Block Size  =");print_hex(64<<((buf[3]>>4) & 0x3));puts("\n\r");
	puts("5th     id  =");print_hex(buf[4]);

	nand_deselect();
	
}

void nand_read(unsigned int addr, unsigned char *buf, unsigned int len)
{

	/* col 是0~0x2FF    (2047)	
	 * page是0~0x1F400 (128k)
	 */
	unsigned int i = 0;
	unsigned int col = addr % 2048;		//相等于addr % 2047
	unsigned int page = addr / 2048;



	nand_select();

	while(i< len)
	{
		nand_cmd(0x00);

		nand_addr_byte((col) & 0xFF);			//col的低8位	
		nand_addr_byte((col>>8) & 0xFF);		//col的高8位	
		nand_addr_byte((page) & 0xFF);			//page的低8位
		nand_addr_byte((page>>8) & 0xFF);		//page的高8位
		nand_addr_byte((page>>16) & 0xFF);		//page的8-16位
		
		nand_cmd(0x30);							//读命令

		nand_wait_ready();
		for (; (col < 2048) && (i < len); col++)
		{
			buf[i++] = nand_data();			
		}

		col = 0;			//读完了一页重置col
		page++;				//读下一页
	}
	nand_deselect();
}

int nand_erase(unsigned int addr)
{
	unsigned int page = addr / 2048;

	nand_select();

	nand_cmd(0x60);		//set up cmd
	
	/* 块擦除，只输入行地址 */
	nand_addr_byte((page) & 0xFF);			//page的低8位
	nand_addr_byte((page>>8) & 0xFF);		//page的高8位
	nand_addr_byte((page>>16) & 0xFF);		//page的8-16位

	nand_cmd(0xD0);		//erase cmd
	nand_wait_ready();

	nand_cmd(0x70);		//获取擦除状态
	if ((nand_data() & 0x01))	//0: success, 1: error
	{
		nand_deselect();
		return -1;
	}
	
	nand_deselect();
	return 0;
}
void nand_write(unsigned char addr, unsigned char *buf, unsigned int len)
{
	unsigned int i = 0;
	unsigned int col = addr & (2048 - 1);		//相等于addr % 2047
	unsigned int page = addr / 2048;
	
	nand_select();

	while(i < len)
	{

		nand_cmd(0x80); 	//set up write cmd
		
		nand_addr_byte((col) & 0xFF);			//col的低8位 
		nand_addr_byte((col>>8) & 0xFF);		//col的高8位 
		nand_addr_byte((page) & 0xFF);			//page的低8位
		nand_addr_byte((page>>8) & 0xFF);		//page的高8位
		nand_addr_byte((page>>16) & 0xFF);		//page的8-16位


		for (; (col < 2048) && (i < len); col++)
		{
			nand_w_data(buf[i++]);
		}
		col = 0;			//写完了一页重置col
		page++;				//读下一页
		
		nand_cmd(0x10);		//end write cmd
		nand_wait_ready();
	}
		
	nand_deselect();
}
void do_read_nand_flash()
{


	unsigned int i = 0, j = 0;
	unsigned char addr, c;
	unsigned char str[16];
	unsigned char buf[64];
	volatile unsigned char *p;

	puts("Input addr: ");
	addr = get_uint();
	
	nand_read(addr, buf, 64);
	p = (volatile unsigned char *)buf;
	for (i=0; i<4; i++)								//打印四行
	{
		for (j=0; j<16; j++)						//每行16个数据
		{
			c = *p++;
			str[j] = c;
			printf("%02x ", c);
			
		}
		puts("   ; ");
		for (j=0; j<16; j++)
		{
			if (str[j] < 0x20 || str[j] > 0x7e)		//不可视字符
			{
				puts(".");
			}
			else									//打印字符
			{
				putchar(str[j]);
			}
		}
		puts("\n\r");								//每行换行
	}

}

void do_erase_nand_flash()
{
	unsigned char addr=0;

	puts("Input addr: ");
	addr = get_uint();	
	
	if (nand_erase(addr) == 0)
	{
		puts("Erase successful.\n\r");
	}
	else
		puts("Erase error.\n\r");
}

void do_write_nand_flash()
{
	unsigned char addr=0;
	unsigned char str[100];

	puts("Input addr: ");
	addr = get_uint();	
	
	puts("Input string: ");
	gets(str);

	puts("Write...\n\r");
	nand_write(addr, str, strlen(str)+1);
}


void nand_flash_test(void)
{
	char c;

	while (1)
	{
		/* 打印菜单, 供我们选择测试内容 */
		puts("\n\r");
		puts("[s] Scan nand flash\n\r");
		puts("[e] Erase nand flash\n\r");
		puts("[w] Write nand flash\n\r");
		puts("[r] Read nand flash\n\r");
		puts("[q] quit\n\r");
		puts("Enter selection: ");

		c = getchar();
		putchar(c);
		puts("\n\r");
		/* 测试内容:
		 * 1. 识别nor flash
		 * 2. 擦除nor flash某个扇区
		 * 3. 编写某个地址
		 * 4. 读某个地址
		 */
		switch (c)		 
		{
			case 'q':
			case 'Q':
				return;
				break;
				
			case 's':
			case 'S':
				read_id();
				break;

			case 'e':
			case 'E':
				do_erase_nand_flash();
				break;

			case 'w':
			case 'W':
				do_write_nand_flash();
				break;

			case 'r':
			case 'R':
				do_read_nand_flash();
				break;
			
			default:
				break;
		}


	}
}
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
		while (dest <= end)
		{
			*dest++ = *src++;	//dest=code_start=0x30000000; src=0
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

void bord_init()
{
#if 0
	bd->bi_boot_params = 0x30000100;
	bd->bi_dram[0].start = 0x30000000;
	bd->bi_dram[0].size = 0x4000000;	//64m
	bd->bi_arch_number = 362;
#endif
	

	timer_init();	//初始化定时器
	led_init();		//初始化LED
}

