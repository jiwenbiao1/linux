#include "pipe.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

/*
 * sem_init semget�������ϵ
 * http://www.360doc.com/content/12/0722/23/9298584_225895213.shtml
 * 	�ڽ�����ʹ��semget ,���߳���ʹ�� sem_init .
 *	sem_init ��posix�ź����������˳����Զ��ͷ�
	semget �Ǽ���ϵͳv���ź�����������ȷ�������ɾ�������Ż��ͷ�
 * http://www.cnblogs.com/linshui91/archive/2010/09/29/1838770.html
 * ���ڵ�linuxͨ�ŷ�ʽ
 * 	1 �ܵ�  �����ܵ�pipe  ���ӽ��̼�� �����ƻ��� �����ȳ�    Ҳ������popen �����ķ�ʽ��
 * 			1 int fd[2] д�ܵ� ���ܵ� fd[0]�� fd[1]д
 * 			2 pipe(fd)
 * 			3 fork() ��������  �����̶�/д ���ӽ���д/��
 * 			4 ע���д˳�� �ܵ�����
 * 			5 ����ʵ�ְ�˫��ͨ��  ��Ҫʵ��ȫ˫��ͨ��  ���Բ������������ܵ�
 * 			6 �رչܵ� close(fd[0])
 * 					close(fd[1])
 * 		�����ܵ� fifo ��ͬ���Ƚ��̼�� ��Ҫ�����ض����ļ��豸�ڵ� �ļ����ȱ��벻����
 * 			1 mkfifo("path",mode)
 * 			2 open("path",)  ע���Ƿ�����
 * 			3  ��д write read
* 	2 �ź�  ���signal  http://blog.csdn.net/ljianhui/article/details/10128731
 * 	3 ��Ϣ����
 * 		http://www.cnblogs.com/lidabo/p/4323807.html
 * 		http://blog.sina.com.cn/s/blog_6334fe7c0100fvtl.html
 * 	4 �����ڴ� http://blog.csdn.net/ljianhui/article/details/10253345
 * 	5 �ź���  semget() http://blog.csdn.net/ljianhui/article/details/10243617
 * 	6 �׽��� http://blog.chinaunix.net/uid-26790551-id-3171897.html
 * *//*
int main(int argc, char **argv) {


	int pipe_fd[2];
	char buffer[30];
	if(pipe(pipe_fd)){
		perror("pipe");
	}

	if(fork()==0)
	{
		printf("child process\n");
		write(pipe_fd[1],"this is child process write",30);
		sleep(5);
		read(pipe_fd[0],buffer,30);//���������ݻ�����
		printf("chilid read  success %s\n",buffer);
	}
	else{
		printf("parent process\n");
		sleep(5);
		read(pipe_fd[0],buffer,30);
		printf("parent read  success %s\n",buffer);
		//write(pipe_fd[1],"this is parent process write",30);

		int status;
		wait(&status);//�����̵ȴ��ӽ��̽���  / waitpid(-1,&status) -1 �����κ��ӽ��� �μ�waitpid����
	}
	close(pipe_fd[0]);
	close(pipe_fd[1]);
	printf("over\n");

	return EXIT_SUCCESS;
}
*/

int main(int argc, char **argv) {

	unlink("fifo.txt");
	/*if(mkfifo("fifo.txt",0777))
	{
		perror("mkfifo");
	}*/
	int fd = open("fifo.txt",O_WRONLY);
	if(fd<0)
	{
		perror("open");
		return EXIT_FAILURE;
	}
	write(fd,"this is write info ",20);
	close(fd);
	return EXIT_SUCCESS;

}
