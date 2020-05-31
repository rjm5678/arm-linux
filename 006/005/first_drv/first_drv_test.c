
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	int val = 1;
	int fd = 0;
	

	fd = open("/dev/first_drv", O_RDWR);
	if (fd<0) printf("can't open file first_drv\n");

	if (argc!=2)
	{
		printf("Usage: first_drv <on/off>\n");
	}
	else 
	{
		if (strcmp(argv[1], "on") == 0)
		{
			val  = 1;
		}
		else
		{
			val = 0;
		}
		write(fd, &val, 4);
	}
	
	
	return 0;
}

