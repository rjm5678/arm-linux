#define BANKCON0 		(*((volatile unsigned int *)0x48000004))

void Bank0_tacc_init(int val)
{
/*
	BANKCON0 &= ~((0x111)<<8);
	BANKCON0 |=  ( (val) <<8);
	*/
		BANKCON0 = val << 8;
}

