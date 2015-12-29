/*
 * main.c
 *
 *  Created on: 2015年12月17日
 *      Author: biao
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#define  SIG_MY 33
/*
 * http://www.cnblogs.com/hoys/archive/2012/08/19/2646377.html
 *
 * signal 几种处理方式
 * 	一、1 signal(int signum , sighandler_t function)
 * 							sighandler_t : void (*_sig_func_ptr)(int); 信号处理函数
 * 								function  自定义处理函数
 * 								SIG_DFL 系统核心处理函数
 * 								SIG_IGN忽略信号
 * 								signal(SIGINT,SIG_DFL);
 * 		2  只能处理signal.h头文件定义的32中信号  其他信号不能处理 ??
 * 二 、  sigaction
 * 		1 struct sigaction act;
 * 		2 act.  初始化
 * 		3 sigatcion(int signum, &act , NULL);注册
 *发送信号的函数
 *		1 kill -s signum pid
 *		2 sigqueue(pid ,signum.sigval);//第三个参数  信号传递的整数值
 * */
void signal_function(int signal_num){
	switch (signal_num) {
		case SIGHUP://KILL -1  pid
			printf("get a signal --- sighup\n");
			break;

		case SIGINT://KILL -2  pid
			printf("get a signal --- sigint\n");
			signal(SIGINT,SIG_DFL); //重新设置为系统默认的核心处理函数((_sig_func_ptr)0)	/* Default action */
			break;
		case SIG_MY://error Unk
				printf("get a signal --- sigmy\n");
				break;
		default:
			break;
	}
}
int main_test(int argc, char **argv) {
/*	int i;
	for ( i = 0; i < 100; ++i) {
		printf("%d\t%s\n",i,strerror(i));
	}*/
	printf("process id = %d\n",getpid());
	signal(SIGHUP,signal_function);
	signal(SIG_MY,signal_function);
	signal(SIGINT,signal_function);
	for ( ;;)
		;

}
