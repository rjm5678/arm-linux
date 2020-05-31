#define GPHCON 		(*((volatile unsigned int *)0x56000070))
#define ULCON0 		(*((volatile unsigned int *)0x50000000))
#define UCON0 		(*((volatile unsigned int *)0x50000004))
#define UTRSTAT0	(*((volatile unsigned int *)0x50000010))
#define UBRDIV0 	(*((volatile unsigned int *)0x50000028))
#define UTXH0	 	(*((volatile unsigned char *)0x50000020))
#define URXH0	 	(*((volatile unsigned char *)0x50000024))
#define GPHUP	 	(*((volatile unsigned char *)0x56000078))


void uart0_config()
{

	/* 配置引脚为TXD和RXD 
	- [7:6] GPH3 设置为RXD[0]：GPHCON[7:6] = 0b10
	- [5:4] GPH2 设置为TXD[0]：GPHCON[5:4] = 0b10
	 */
	GPHCON &= ~((3<<5)|(3<<7)); //清零
	GPHCON |=  ((1<<5)|(1<<7)); //置一
	
	GPHUP &= ~((1<<2) | (1<<3));  /* 使能内部上拉 */

	/* 配置ULCON0
	 - [6]	 Infrared Mode 为正常模式：ULCON0[6] = 0
	 - [5:3] Parity Mode 没有校验位（简单嘛）：ULCON0[5:3] = 0xx
	 - [2]	 Number of Stop Bit 每帧一个停止位 ： ULCON0[2] = 0
	 - [1:0] Word Length 数据位数为8位： ULCON0[1:0] = 0b11
	 - 也就是 ULCON0 = 0b11 = 0x3
	*/
	ULCON0 = 3;

	/* 配置UCON0
	- [15:12]  FCLK Divider 不需要用，所以就不设置吧，默认0，不用
	- [11:10]  Clock Selection 选择PCLK ： UCON0[11:10] = 00 or 10
	- [9] Tx Interrupt Type 默认0，不用
	- [8] Rx Interrupt Type 默认0，不用
	- [7] Rx Time Out Enable 默认0，不用
	- [6] Rx Error Status Interrupt Enable 默认0，不用
	- [5] Loopback Mode 默认0，不用
	- [4] Send Break Signal 默认0，不用
	- [3:2] Transmit Mode 设置为中断模式，没其他可以选了：UCON0[3:2] = 0b01
	- [1:0] Receive Mode 设置为中断模式，没其他可以选了：UCON0[1:0] = 0b01
	
	**即 UCON0 = 0x5 or 0x805**
	*/
	UCON0 = 0x5;

	/* 配置波特率UBRDIVn
	 * UBRDIVn = (int)( 50 000 000 / ( 115200 x 16) ) –1 = 26.12 ≈ 26 
	 */
	UBRDIV0 = 26;
	
}

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


