#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
	unsigned char keyval[4] = {1,1,1,1};
	int fd = 0;
	
	/* 打开驱动文件 */
	fd = open("/dev/key_drv", O_RDWR);
	if (fd<0) printf("can't open file key_drv\n");

	while(1)
	{
		read(fd, keyval, sizeof(keyval));
		if ( !keyval[0] || !keyval[1] || !keyval[2] || !keyval[3] )
		{
			printf("%d %d %d %d\n", keyval[0], keyval[1], keyval[2], keyval[3]);
		}
	}
	return 0;
}

