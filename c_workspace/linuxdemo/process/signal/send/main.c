#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <error.h>
#include <err.h>


int main_test(int argc, char **argv) {
	pid_t pid;
	int signum;
	if (argc != 3) {
		errno = EINVAL;//全局变量参数不正确
		perror("argument");
		return EXIT_FAILURE;
	}
	signum = atoi(argv[1]);
	pid = atoi(argv[2]);
	sigval_t sigval;
	sigval.sival_int = 125;//传递的整数
	if(sigqueue(pid,signum,sigval)){
		perror("sigqueue");
	}


	return EXIT_SUCCESS;
}
