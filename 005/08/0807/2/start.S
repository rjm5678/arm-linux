
.text
.global _start

_start:

	ldr sp ,= 4096 /*nand 启动*/

	mov r0, #4
	bl led_on /*点亮GPF4->LED1*/
	
	ldr r0, =100000
	bl delay
	
	mov r0, #5
	bl led_on
	
halt:
	b halt