void print_und_exception(char* a, char* b)
{
	puts("\n\rcpsr = ");
	print_hex(a);
	puts("\n\r");

	puts(b);
}

void print_swi_exception(char* a, char* b)
{
	puts("\n\rcpsr = ");
	print_hex(a);
	puts("\n\r");

	puts(b);
}
void print_swi_val(unsigned int *pswi)
{
	puts("\n\rswi_val = ");
	print_hex(*pswi & ~(0xff000000));
	puts("\n\r");
}

