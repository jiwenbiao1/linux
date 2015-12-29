/*
 * main.c
 *
 *  Created on: 2015��12��17��
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
 * signal ���ִ���ʽ
 * 	һ��1 signal(int signum , sighandler_t function)
 * 							sighandler_t : void (*_sig_func_ptr)(int); �źŴ�����
 * 								function  �Զ��崦����
 * 								SIG_DFL ϵͳ���Ĵ�����
 * 								SIG_IGN�����ź�
 * 								signal(SIGINT,SIG_DFL);
 * 		2  ֻ�ܴ���signal.hͷ�ļ������32���ź�  �����źŲ��ܴ��� ??
 * �� ��  sigaction
 * 		1 struct sigaction act;
 * 		2 act.  ��ʼ��
 * 		3 sigatcion(int signum, &act , NULL);ע��
 *�����źŵĺ���
 *		1 kill -s signum pid
 *		2 sigqueue(pid ,signum.sigval);//����������  �źŴ��ݵ�����ֵ
 * */
void signal_function(int signal_num){
	switch (signal_num) {
		case SIGHUP://KILL -1  pid
			printf("get a signal --- sighup\n");
			break;

		case SIGINT://KILL -2  pid
			printf("get a signal --- sigint\n");
			signal(SIGINT,SIG_DFL); //��������ΪϵͳĬ�ϵĺ��Ĵ�����((_sig_func_ptr)0)	/* Default action */
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
