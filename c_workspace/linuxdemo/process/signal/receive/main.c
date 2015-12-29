
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>

void sigal_function(int signum,siginfo_t* info,void* myaction)
{
	printf("receive signum is %d\n",signum);
}
int main_test(int argc, char **argv) {

	struct sigaction act;
	int signum;
	signum = atoi(argv[1]);
	act.sa_sigaction = sigal_function;
	sigemptyset(&act.sa_mask);
	act.sa_flags = SA_SIGINFO;
	if(sigaction(signum,&act,NULL)){
		perror("sigaction");//×¢²ásigaction
	}
	printf("sig = %d",signum);
	printf("pid = %d\n",getpid());
	while(1)
	{
		sleep(1);
		printf("wait signal \n");
	}

}
