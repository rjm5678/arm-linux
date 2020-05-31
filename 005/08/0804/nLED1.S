.text
.global _start

_start:
	
	LDR R1, =0x56000050/*GPFCON寄存器*/
	LDR R0, =0x400
	STR R0, [R1]/*配置GPF5为输出*/

	LDR R1, =0x56000054/*GPFDAT寄存器*/
	LDR R0, =0x000
	STR R0, [R1]	/*配置nLED2为0*/

halt:
	b halt
		
