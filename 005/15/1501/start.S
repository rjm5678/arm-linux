
.text
.global _start

_start:
	b reset	/* 0x30000000 */
//	ldr pc, =do_und	/* 0x30000004 */
	ldr pc, und_addr	
	ldr pc, swi_addr
	b halt			 /* vector 0x0c : prefetch aboot */
	b halt			 /* vector 0x10 : data abort */
	b halt			 /* vector 0x14 : reserved */
	ldr pc, irq_addr /* vector 0x18 : irq */
	b halt			 /* vector 0x1c : fiq */


/* 这里为什么这么做
 * ldr pc, und_addr 强制加载到这里，这里再强制加载到 do_und
 * 如果不是，由于"ldr pc, =do_und" 是一条伪指令，do_und这个地址就会被编译器默认保存在编译文件最后的地址
 * 修改后就能达到，直接跳到接着上一句的地址执行，而不用跳到最后的地址
 */
und_addr:
	.word do_und

swi_addr:
	.word do_swi

irq_addr:
	.word do_irq


do_und:
	/* 重要！栈未设置，需要重新设置，指向一个不会被用到的地方 */
	ldr sp, =0x34000000
	
	/* 保护现场 */
	stmdb sp!, {r0-r12, lr}			//目前位置 lr 里面有被打断前的下一条将要执行的指令，所以也要保存
	
	/* 处理异常 */
	mrs r0, cpsr	//读cpsr的值，作为print_und_exception的第一个参数
	ldr r1, =und_string	//r1 载入字符串，作为print_und_exception的第二个参数
	bl print_und_exception

	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^	/* ^会把spsr的值恢复到cpsr里 */

und_string:
	.string "und exception!!!"

.align (4)	// 前面是字串 后面必须4字节对气 


do_swi:
	/* 重要！栈未设置，需要重新设置，指向一个不会被用到的地方 */
	ldr sp, =0x33e00000
	
	/* 保护现场 */
	stmdb sp!, {r0-r12, lr}			//目前位置 lr 里面有被打断前的下一条将要执行的指令，所以也要保存

	/* 调用c函数会破坏lr寄存器，所以要提前保护
 	 * 根据ATPCS规则，c函数调用完会恢复r4-r11的寄存器，所以可以r4来保存
	 */
	mov r4, lr	
	
	/* 处理异常 */
	mrs r0, cpsr					//读cpsr的值，作为print_swi_exception的第一个参数
	ldr r1, =swi_string				//r1 载入字符串，作为print_swi_exception的第二个参数
	bl print_swi_exception

	sub r0, r4, #4					/* lr是保存异常前下一条指令的地址，减4后，r0就是swi异常的句子*/
	bl print_swi_val

	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^		/* ^会把spsr的值恢复到cpsr里 */
	
swi_string:
	.string "swi exception!!!"

.align (4)	// 前面是字串，后面必须4字节对齐


do_irq:
	/* 重要！栈未设置，需要重新设置，指向一个不会被用到的地方 */
	ldr sp, =0x33d00000

	/* 保护现场 */
	sub lr, lr, #4					//根据lr恢复规则，异常前的lr-4 = 异常后的lr
	stmdb sp!, {r0-r12, lr}			//目前位置 lr 里面有被打断前的下一条将要执行的指令，所以也要保存
	
	/* 处理异常 */
	bl handle_irq_c

	/* 恢复现场 */
	ldmia sp!, {r0-r12, pc}^		/* ^会把spsr的值恢复到cpsr里 */
	
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

usr:
	/* 复位之后, cpu处于svc模式
	 * 现在, 切换到usr模式
	 */
	mrs r0, cpsr      /* 读出cpsr */
	bic r0, r0, #0xf  /* 修改M4-M0为0b10000, 进入usr模式 */
	bic r0, r0, #(1<<7)  /* 重要！清除I位, 使能中断 */
	msr cpsr, r0

	/* 设置 sp_usr */
	ldr sp, =0x33f00000

	/* 进入sdram */
	ldr pc, =sdram
sdram:
	bl uart0_init	/* 初始化串口 */
	bl print123

und:
	.word 0xdeadc0de	/* 故意加入未定义句子 */

	bl printABC

swi:
	swi 0x123


//	bl main
	ldr pc, =main	/*  */
	
halt:
	b halt
