#ifndef S3C2440_SOC_H
#define S3C2440_SOC_H


#define BANKCON0 		(*((volatile unsigned int *)0x48000004))

#define BWSCON 		(*((volatile unsigned int*)(0x48000000)))
#define BANKCON6 	(*((volatile unsigned int*)(0x4800001C)))
#define REFRESH 	(*((volatile unsigned int*)(0x48000024)))
#define BANKSIZE 	(*((volatile unsigned int*)(0x48000028)))
#define MRSRB6 		(*((volatile unsigned int*)(0x4800002C)))

#define GPHCON 		(*((volatile unsigned int *)0x56000070))
#define ULCON0 		(*((volatile unsigned int *)0x50000000))
#define UCON0 		(*((volatile unsigned int *)0x50000004))
#define UTRSTAT0	(*((volatile unsigned int *)0x50000010))
#define UBRDIV0 	(*((volatile unsigned int *)0x50000028))
#define UTXH0	 	(*((volatile unsigned char *)0x50000020))
#define URXH0	 	(*((volatile unsigned char *)0x50000024))
#define GPHUP	 	(*((volatile unsigned char *)0x56000078))



#define SRCPND 			(*((volatile unsigned int *)0X4A000000))
#define INTPND 			(*((volatile unsigned int *)0X4A000010))
#define INTMSK			(*((volatile unsigned int *)0X4A000008))
#define INTOFFSET  		(*((volatile unsigned int *)0x4A000014))

#define GPFCON 			(*((volatile unsigned int *)0X56000050))
#define GPFDAT 			(*((volatile unsigned int *)0X56000054))
#define GPGCON 			(*((volatile unsigned int *)0X56000060))
#define GPGDAT 			(*((volatile unsigned int *)0X56000064))

#define EXTINT0 			(*((volatile unsigned int *)0x56000088))
#define EXTINT1 			(*((volatile unsigned int *)0x5600008c))
#define EXTINT2 			(*((volatile unsigned int *)0x56000090))
#define EINTMASK 			(*((volatile unsigned int *)0x560000a4))
#define EINTPEND 			(*((volatile unsigned int *)0x560000a8))

/* LED寄存器 */
#define C_LED1 8  //GPF4
#define C_LED2 10 //GPF5
#define C_LED3 12 //GPF6

#define D_LED1 4  //GPF4
#define D_LED2 5  //GPF5
#define D_LED3 6  //GPF6

/* 按键寄存器 */
#define C_S2 0 /* GPF0 */
#define C_S3 4 /* GPF2 */
#define C_S4 6 /* GPG3 */
#define C_S5 22 /* GPG11 */

#define D_S2 0 /* GPF0 */
#define D_S3 2 /* GPF2 */
#define D_S4 3 /* GPG3 */
#define D_S5 11 /* GPG11 */

#endif 

