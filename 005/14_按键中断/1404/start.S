
.text
.global _start

_start:
	b reset	/* 0x30000000 */
//	ldr pc, =do_und	/* 0x30000004 */
	ldr pc, und_addr	

/* 这里为什么这么做
 * ldr pc, und_addr 强制加载到这里，这里再强制加载到 do_und
 * 如果不是，由于"ldr pc, =do_und" 是一条伪指令，do_und这个地址就会被编译器默认保存在编译文件最后的地址
 * 修改后就能达到，直接跳到接着上一句的地址执行，而不用跳到最后的地址
 */
und_addr:
	.word do_und

do_und:
	/* 重要！栈未设置，需要重新设置，指向一个不会被用到的地方 */
	ldr sp, =0x34000000
	
	/* 保护现场 */
	stmdb sp!, {r0-r12, lr}			//目前位置 lr 里面有被打断前的下一条将要执行的指令，所以也要保存
	
	/* 处理异常 */
	mrs r0, cpsr	//读cpsr的值，作为print_und_exception的第一个参数
	ldr r1, =und_string	//r1 载入字符串，作为print_und_exception的第二个参数
	ldr pc, =print_und_exception

	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^	/* ^会把spsr的值恢复到cpsr里 */

und_string:
	.string "und exception!!!"

.align (4)	// 4 zijie duiqi

reset:
	/* 关闭看门狗 */
	ldr r0, =0x53000000
	ldr r1, =0
	str r1, [r0]  /* 把WTCON第零位设置为0，关看门狗 */

	/* 设置为异步模式 */
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#0xC0000000	//R1_nF:OR:R1_iA
	mcr p15,0,r0,c1,c0,0

	/* 设置CLKDIVN寄存器使得 FCLK:HCLK:PCLK = 1:4:8
	 * CLKDIVN寄存器 地址：0x4C000014
	 * HDIVN [2:1] : 0b10
	 * PDIVN [0] : 0b1
	 * 即：[0x4C000014] = 0b101 = 0x5
	 */
	ldr r0, =0x4C000014
	ldr r1, =0x5
	str r1, [r0]


	
	/* 设置PLLCON使FLCK = 400MHz
	 * PLLCON寄存器 地址 :  0x4C000004
	 * MDIV  [19:12] : 0x5C = 92
	 * PDIV [9:4]	 : 1
	 * SDIV [1:0]    : 1
	 * 即：[0x4C000004] = 0b101 1100 0000 0001 0001‬ = 0x5C011
	 */
	ldr r0, =0x4C000004
	ldr r1, =0x5C011
	str r1, [r0]


	/* 设置sp栈，自动识别nor启动还是nand启动
	 * nor启动时对应nor flash，nand启动时对应片内内存
	 * 在nor flash 写数据时必须有一定格式
	 * 把0写入0地址，再读出来看有没有修改，有修改就是nand启动，没修改就是nor启动
	 */
	
	mov r1, #0
	ldr r0, [r1] /* 先把[0]地址的值备份到r0 */
	str r1, [r1] /* 把0写入[0]地址 */
	ldr r2, [r1] /* 再把[0]地址的值读出来 */
	cmp r1,r2    /* 比较 */
	ldr sp, =0x40000000+4096 /* nor启动 */
	moveq sp , #4096 /*nand 启动*/
	streq r0, [r1] /* 恢复数据 */

	bl sdram_init	/* 初始化sdram */
	
	bl copy2sdram	/* 重定位代码 */
	
	bl clean_bss	/* 清零bss段 */

	ldr pc, =sdram
sdram:
	bl uart0_config	/* 初始化串口 */
	bl print123
und:
	.word 0xabcfddde	/* 故意加入未定义句子 */
	bl printABC
//	bl main
	ldr pc, =main	/*  */
	
halt:
	b halt
