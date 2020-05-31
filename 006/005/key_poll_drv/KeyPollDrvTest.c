#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <linux/poll.h>


int main(int argc, char **argv)
{
	unsigned char keyval;
	int fd = 0;
	
	
	struct pollfd fds[1];
	
	/* 打开驱动文件 */
	fd = open("/dev/buttons", O_RDWR);
	if (fd<0) {
		printf("can't open file key_drv\n");
		return -1;
	}

	fds[0].fd = fd;
	fds[0].events = POLLIN;	//输入时唤醒
	
	while(1)
	{
		if ( ! poll(fds, 1, 5000) )
		{
			printf("time out, ret = 0 \n");
		}
		else{
			read(fd, &keyval, 1);
			printf("keyval:0x%x\n", keyval);
		}	
	}
	return 0;
}

