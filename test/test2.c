#include <stdio.h>

int func(char* p1, char *p2)
{
	printf("*p1 = %s, *p2 = %s\n", p1, p2);
	
	while(*p1 != '\0')
	{
		if( *p1++ != *p2++)
		{
			return 1;
		}
	}
	
	return 0;
}

int main(int argc, char **argv)
{
	int i;
	
	i = func(argv[1], argv[2]);
	printf("%d\n", i);

	
	return 0;
}
