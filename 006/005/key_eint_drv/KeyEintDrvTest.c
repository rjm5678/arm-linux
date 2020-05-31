#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	unsigned char keyval;
	int fd = 0;
	
	/* 打开驱动文件 */
	fd = open("/dev/key_eint_drv", O_RDWR);
	if (fd<0) printf("can't open file key_drv\n");

	while(1)
	{
		read(fd, &keyval, 1);
		printf("keyval:0x%x\n", keyval);
	}
	return 0;
}

