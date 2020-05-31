.text
.global _start

_start:

	/* 关闭看门狗 */
	ldr r0, =0x53000000
	ldr r1, =0
	str r1, [r0]  /* 把WTCON第零位设置为0，关看门狗 */

	/* 设置sp栈，自动识别nor启动还是nand启动
	 * nor启动时对应nor flash，nand启动时对应片内内存
	 * 在nor flash 写数据时必须有一定格式
	 * 把0写入0地址，再读出来看有没有修改，有修改就是nand启动，没修改就是nor启动
	 */
	 
	ldr sp ,= 4096 /*nand 启动*/
	
	mov r1, #0
	ldr r0, [r1] /* 先把[0]地址的值备份到r0 */
	str r1, [r1] /* 把0写入[0]地址 */
	ldr r2, [r1] /* 再把[0]地址的值读出来 */
	cmp r1,r2    /* 比较 */
	ldr sp, =0x400000000+4096 /* nor启动 */
	moveq sp , #4096 /*nand 启动*/
	streq r0, [r1] /* 恢复数据 */
	
	bl main
	
halt:
	b halt