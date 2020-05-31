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
unsigned int nor_read_word(unsigned int base, unsigned int offset)
{	
	volatile unsigned short *paddr = (volatile unsigned short *)(base + (offset << 1)); 	//硬件上，CPU 发出的地址给nor的都是(>>1)，需要修正

	return *paddr;
}

/* 往nor读数据 */
unsigned int nor_dat(unsigned int addr)
{
	return nor_read_word(NOR_FLASH_BASE, addr);
}
void nor_wait_ready(unsigned int addr)
{
	unsigned int val;
	unsigned int pre;

	pre = nor_dat(addr>>1);
	val = nor_dat(addr>>1);
	while ((val & (1<<6)) != (pre & (1<<6)))
	{
		pre = val;
		val = nor_dat(addr>>1);		
	}
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
	puts("vendor id: "); print_hex(nor_dat(0)); puts("\n\r");	//0xC2
	puts("device id: "); print_hex(nor_dat(1)); puts("\n\r");	//0x2249
	
	
	
	/* 要读取设备信息，首先需要进入CFI 模式
	 * 进入CFI 的指令是 [55H] = 0x98
	 */
	nor_cmd(0x55, 0x98);
#if 0
	str[1] = nor_dat(0x10); putchar(str[1]);
	str[2] = nor_dat(0x11); putchar(str[2]);
	str[3] = nor_dat(0x12); putchar(str[3]);
	str[4] = '\0';
#endif
	
	putchar(nor_dat(0x10));	//Q
	putchar(nor_dat(0x12));	//R
	putchar(nor_dat(0x11));	//Y
	puts(" CFI mode... \n\r");
	puts("nor size: ");  printf("0x%x",(unsigned int)(1<<(nor_dat(0x27))) / 1024 / 1024);puts("M\n\r");
	
	region  = nor_dat(0x2C);
	puts("region  : ");  print_hex(region);puts("\n\r");

	/*
	> 参考CFI publication 100基准，可知扇区信息怎么解读：
	> - 前两字节+1       = 该region的扇区个数
	> - 后两字节*256 = 该region的扇区大小
	在data(h) 只是取低8位的数据
	*/

	for (i=0; i<region; i++)
	{
		sectors = 1 + (nor_dat(region_info_base + 0) + (nor_dat(region_info_base + 1) << 8));	
		sector_size = 256 * (nor_dat(region_info_base + 2) + (nor_dat(region_info_base + 3) << 8));
		region_info_base += 4;
		
		for(j=0; j<sectors; j++)
		{
			printf("0x%x",sector_addr);putchar(' ');
			sector_addr += sector_size;
			cnt++;
			if (cnt % 5 == 0)
				puts("\n\r");
		}
		
	}
	
	nor_cmd(0, 0xF0); //重置，退出CFI模式
}
void do_erase_nor_flash()
{
	unsigned int addr;
		
		/* 获得地址 */
		printf("Enter the address of sector to erase: ");
		addr = get_uint();
	
		printf("erasing ...\n\r");
		nor_cmd(0x555, 0xaa);	 /* 解锁 */
		nor_cmd(0x2aa, 0x55); 
		nor_cmd(0x555, 0x80);	 /* erase sector */
		
		nor_cmd(0x555, 0xaa);	 /* 解锁 */
		nor_cmd(0x2aa, 0x55); 
		nor_cmd(addr>>1, 0x30);  /* 发出扇区地址 */
		nor_wait_ready(addr);

}
void do_write_nor_flash()
{
	unsigned int addr;
	unsigned char str[100];
	int i, j;
	unsigned int val;
	
	/* 获得地址 */
	printf("Enter the address of sector to write: ");
	addr = get_uint();

	printf("Enter the string to write: ");
	gets(str);

	printf("writing ...\n\r");

	/* str[0],str[1]==>16bit 
	 * str[2],str[3]==>16bit 
	 */
	i = 0;
	j = 1;
	while (str[i] && str[j])
	{
		val = str[i] + (str[j]<<8);
		
		/* 烧写 */
		nor_cmd(0x555, 0xaa);	 /* 解锁 */
		nor_cmd(0x2aa, 0x55); 
		nor_cmd(0x555, 0xa0);	 /* program */
		nor_cmd(addr>>1, val);
		/* 等待烧写完成 : 读数据, Q6无变化时表示结束 */
		nor_wait_ready(addr);

		i += 2;
		j += 2;
		addr += 2;
	}

	val = str[i];
	/* 烧写 */
	nor_cmd(0x555, 0xaa);	 /* 解锁 */
	nor_cmd(0x2aa, 0x55); 
	nor_cmd(0x555, 0xa0);	 /* program */
	nor_cmd(addr>>1, val);
	/* 等待烧写完成 : 读数据, Q6无变化时表示结束 */
	nor_wait_ready(addr);
}

void do_read_nor_flash()
{
	unsigned int i = 0, j = 0;
	unsigned int addr, c;
	unsigned char str[16];
	unsigned char buf[64];
	volatile unsigned char *p;


	puts("Input addr: ");
	addr = get_uint();

	for (i=0;i<64;i++)
	{
		buf[i] = nor_dat(addr++);
	}
	
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

		

	}
}


