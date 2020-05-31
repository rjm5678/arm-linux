
#include <stdio.h>

int strcmp(char *p1, char *p2)
{
	printf("12");
	printf("*p1 = %s, *p2 = %s\n", p1, p2);
	
	return 1;
}

int main()
{
	int i= 10;
	i = strcmp("abc", "abc");

	
	printf("%d\n",i);

	

	return 0;
}
