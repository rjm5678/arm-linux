#include <stdio.h>
#include <signal.h>


void my_signal_func(int signum)
{
	static int n = 0;
	printf("signal = %d, times = %d\n", signum, ++n);
}

int main()
{

	signal(SIGUSR1, my_signal_func);

	while(1)
	{
		sleep(1000);
	}

	return 0;
}





