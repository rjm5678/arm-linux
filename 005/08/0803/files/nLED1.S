.text
.global _start

_start:
	
	ldr r1, =0x56000050  /*GPFCON寄存器*/
	ldr r0, =0x100
	str r0, [r1]         /*配置GPF4为输出*/

	ldr r1, =0x56000054  /*GPFDAT寄存器*/
	ldr r0, =0x0000     
	str r0, [r1]         /*配置nLED1为0*/

halt:
	b halt
		
