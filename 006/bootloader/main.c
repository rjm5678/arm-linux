#include "s3c2440_soc.h"


int main()
{
	void (*theKernel)(int zero, int arch, unsigned int params);

	uart0_init();
//	nand_flash_test();
	/* 把内核从Nand flash读进内存 */ 
	/* 0x60000+64为内核开始地址，0x64是Uimage
	 * 读到0x300008000去，u-boot都这么做，约定俗成
	 * 读2兆
	 */	
	puts("Copy Kernal from Nand...\n\r");
	nand_read(0x60000+64, (unsigned char *)0x30008000, 0x200000);


	/* 设置tag参数 */
	puts("Setting Parm for Kernal...\n\r");
	setup_start_tag();
	setup_memory_tags();
	setup_commandline_tag( "noinitrd root=/dev/mtdblock3 init=/linuxrc console=ttySAC0");
	setup_end_tag();

	/* 跳转执行 */
	puts("Booting Kernal...\n\r");
	theKernel = (void (*)(int, int, unsigned int))0x30008000;
	theKernel(0, 362, 0x30000100);

	puts("Error!");
}






