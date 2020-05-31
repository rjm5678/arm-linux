#define BWSCON 		(*((volatile unsigned int*)(0x48000000)))
#define BANKCON6 	(*((volatile unsigned int*)(0x4800001C)))
#define REFRESH 	(*((volatile unsigned int*)(0x48000024)))
#define BANKSIZE 	(*((volatile unsigned int*)(0x48000028)))
#define MRSRB6 		(*((volatile unsigned int*)(0x4800002C)))

void sdram_init()
{

	/*
	 * BWSCON 设置宽度和等待状态的寄存器：0x48000000*
	 - [27] ST6：0 = Not using UB/LB (The pins are dedicated nWBE[3:0 )
	 - [26] WS6：0 = WAIT disable
	 - [25:24] DW6：10 = 32-bit	//32位宽
	 [0x48000000]= 0x200 0000
	*/
	BWSCON = 0x2000000;

	/*
	 *BANKCON6	BANK6控制寄存器 地址：0x4800001C** 
	 - [16:15] MT： 0x11= Sync. DRAM
	 - [3:2] Trcd：00 = 2 clocks	//RAS to CAS delay，读完行再读列的延时时间，根据SDRAM芯片手册，2clk = 2 *（1/100MHZ）=20ns
	 - [1:0] SCAN：01 = 9-bit //Column address number，根据SDRAM芯片手册
	 * BANKCON6 = 0x18001
	 */
	BANKCON6 = 0x18001;

	/*
	 **REFRESH：0x48000024  //SDRAM refresh control register**
	  
	  - [23] REFEN：1 = Enable (self or CBR/auto
	 refresh)
	  - [22] TREFMD：0 = CBR/Auto Refresh 
	  - [21:20] Trp：00 = 2 clocks //根据SDRAM芯片手册，2clk = 2 *（1/100MHZ）=20ns
	  - [19:18] Tsrc：01 = 5 clocks //根据SDRAM芯片手册， Tsrc = Trc - Trp = 7-2（clk）=5（clk）
	  - [10:0] Refresh Counter：公式2^11 + 1 - 100x7.8 = 1269 = 0x4F5 //根据SDRAM芯片手册，刷新周期64ms/8192 ≈ 7.8ms
	 REFRESH = 0x8404F5
	 */
	 REFRESH = 0x8404F5;

	 /*
	  **BANKSIZE：0x48000028 //Flexible bank size register**
	  
	   - [7] BURST_EN：1 = Disable burst operation.
	   - [5] SCKE_EN：1 = SDRAM power down mode disable
	   - [4] SCLK_EN：1 = SCLK is active only during the access (recommended).
	   - [2:0] BK76MAP：001 = 64MB/64MB
	  BANKSIZE = 0xb1

	  */
	  BANKSIZE = 0xb1;

	  /*
	   **MRSRB6：0x4800002C   //Mode register set register bank6** 
		- [9] WBL：0: Burst (Fixed)
		- [8:7] TM：00: Mode register set (Fixed)
		- [6:4] CL：010 = 2 clocks    //根据SDRAM芯片手册
		- [3] BT：0: Sequential (Fixed)
		- [2:0] BL：000: 1 (Fixed)    //内存控制器给SDRAM发出地址后，SDRAM需要等待多久才返回数据
	   MRSRB6 = 0x20

	   */
	   MRSRB6 = 0x20;
}

int sdram_test()
{
	volatile unsigned char *p = (volatile unsigned char *)0x30000000;
	int i;

	// write sdram
	for (i = 0; i < 1000; i++)
		p[i] = 0x55;

	// read sdram
	for (i = 0; i < 1000; i++)
		if (p[i] != 0x55)
			return -1;

	return 0;
}

