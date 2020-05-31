#include "s3c2440_soc.h"

#define NOR_FLASH_BASE 0

/* 往nor写2字节数据 */
void nor_write_word(unsigned int base, unsigned int offset, unsigned int val)
{
	volatile unsigned short *paddr = (volatile unsigned short *)(base + (offset << 1));		//硬件上，CPU 发出的地址给nor的都是(>>1)，需要修正

	*paddr = val;
}
/* 往nor写命令 */
void nor_cmd(unsigned int addr, unsigned int cmd)
{
	nor_write_word(NOR_FLASH_BASE, addr, cmd);
}

/* 往nor读2字节数据 */
unsigned short nor_read_word(unsigned int base, unsigned int offset)
{	
	volatile unsigned short *paddr = (volatile unsigned short *)(base + (offset << 1)); 	//硬件上，CPU 发出的地址给nor的都是(>>1)，需要修正

	return *paddr;
}

/* 往nor读数据 */
unsigned int nor_read(unsigned int addr)
{
	return nor_read_word(NOR_FLASH_BASE, addr);
}

/* 解锁命令 */
void unlock_cmd()
{
	nor_cmd(0x555, 0xAA);
	nor_cmd(0x2AA, 0x55);
}
void do_scan_nor_flash()
{
	unsigned int i = 0, j = 0, cnt = 0;
	unsigned int region = 0, sectors = 0, sector_size = 0, sector_addr = 0;
	unsigned int region_info_base = 0x2D;

	unlock_cmd();	//解锁
	nor_cmd(0x555, 0x90);
	puts("\n\r");
	puts("vendor id: "); print_hex(nor_read(0)); puts("\n\r");	//0xC2
	puts("device id: "); print_hex(nor_read(1)); puts("\n\r");	//0x2249
	
	
	
	/* 要读取设备信息，首先需要进入CFI 模式
	 * 进入CFI 的指令是 [55H] = 0x98
	 */
	nor_cmd(0x55, 0x98);
#if 0
	str[1] = nor_read(0x10); putchar(str[1]);
	str[2] = nor_read(0x11); putchar(str[2]);
	str[3] = nor_read(0x12); putchar(str[3]);
	str[4] = '\0';
#endif
	
	putchar(nor_read(0x10));	//Q
	putchar(nor_read(0x12));	//R
	putchar(nor_read(0x11));	//Y
	puts(" CFI mode... \n\r");
	puts("nor size: ");  print_hex((1<<(nor_read(0x27))) / 1024 / 1024);puts("M\n\r");
	
	region  = nor_read(0x2C);
	puts("region  : ");  print_hex(region);puts("\n\r");

	/*
	> 参考CFI publication 100基准，可知扇区信息怎么解读：
	> - 前两字节+1       = 该region的扇区个数
	> - 后两字节*256 = 该region的扇区大小
	在data(h) 只是取低8位的数据
	*/

	for (i=0; i<region; i++)
	{
		sectors = 1 + (nor_read(region_info_base + 0) + (nor_read(region_info_base + 1) << 8));	
		sector_size = 256 * (nor_read(region_info_base + 2) + (nor_read(region_info_base + 3) << 8));
		region_info_base += 4;
		
		for(j=0; j<sectors; j++)
		{
			print_hex_with0(sector_addr);putchar(' ');
			sector_addr += sector_size;
			cnt++;
			if (cnt % 5 == 0)
				puts("\n\r");
		}
		
	}

	


}
void do_erase_nor_flash()
{

}
void do_write_nor_flash()
{
	
}
void do_read_nor_flash(unsigned int addr)
{
	
}

void nor_flash_test(void)
{
	char c;

	while (1)
	{
		/* 打印菜单, 供我们选择测试内容 */
		puts("[s] Scan nor flash\n\r");
		puts("[e] Erase nor flash\n\r");
		puts("[w] Write nor flash\n\r");	
		puts("[r] Read nor flash\n\r");
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
				do_scan_nor_flash();
				break;

			case 'e':
			case 'E':
				do_erase_nor_flash();
				break;

			case 'w':
			case 'W':
				do_write_nor_flash();
				break;

			case 'r':
			case 'R':
				do_read_nor_flash();
				break;
			default:
				break;
		}

		nor_cmd(0, 0xF0); //重置，退出CFI模式


	}
}


