#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <linux/poll.h>
#include <signal.h>

int fd;

void my_signal_fun(int signum)
{
	unsigned char key_val;
	
	read(fd, &key_val, 1);
	printf("key_val: 0x%x\n", key_val);
}

int main(int argc, char **argv)
{
	unsigned char keyval;
	int Oflags;
	
	signal(SIGIO, my_signal_fun);

	/* 打开驱动文件 */
	fd = open("/dev/buttons", O_RDWR);
	if (fd<0) {
		printf("can't open file key_drv\n");
		return -1;
	}

	
	fcntl(fd, F_SETOWN, getpid());
	
	Oflags = fcntl(fd, F_GETFL); 
	
	fcntl(fd, F_SETFL, Oflags | FASYNC);


	while (1)
	{
		sleep(1000);
	}


	return 0;
}

