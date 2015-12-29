#include "pipe.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

/*
 * sem_init semget区别和联系
 * http://www.360doc.com/content/12/0722/23/9298584_225895213.shtml
 * 	在进程中使用semget ,在线程中使用 sem_init .
 *	sem_init 是posix信号量，进程退出会自动释放
	semget 是兼容系统v的信号量，必须明确对其进行删除操作才会释放
 * http://www.cnblogs.com/linshui91/archive/2010/09/29/1838770.html
 * 现在的linux通信方式
 * 	1 管道  无名管道pipe  父子进程间的 流控制机制 先入先出    也可以用popen 以流的方式打开
 * 			1 int fd[2] 写管道 读管道 fd[0]读 fd[1]写
 * 			2 pipe(fd)
 * 			3 fork() 创建进程  父进程读/写 、子进程写/读
 * 			4 注意读写顺序 管道阻塞
 * 			5 可以实现半双工通道  想要实现全双工通道  可以采用两个无名管道
 * 			6 关闭管道 close(fd[0])
 * 					close(fd[1])
 * 		命名管道 fifo 不同祖先进程间的 需要生成特定的文件设备节点 文件事先必须不存在
 * 			1 mkfifo("path",mode)
 * 			2 open("path",)  注意是否阻塞
 * 			3  读写 write read
* 	2 信号  详见signal  http://blog.csdn.net/ljianhui/article/details/10128731
 * 	3 消息队列
 * 		http://www.cnblogs.com/lidabo/p/4323807.html
 * 		http://blog.sina.com.cn/s/blog_6334fe7c0100fvtl.html
 * 	4 共享内存 http://blog.csdn.net/ljianhui/article/details/10253345
 * 	5 信号量  semget() http://blog.csdn.net/ljianhui/article/details/10243617
 * 	6 套接字 http://blog.chinaunix.net/uid-26790551-id-3171897.html
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
		read(pipe_fd[0],buffer,30);//读不到数据会阻塞
		printf("chilid read  success %s\n",buffer);
	}
	else{
		printf("parent process\n");
		sleep(5);
		read(pipe_fd[0],buffer,30);
		printf("parent read  success %s\n",buffer);
		//write(pipe_fd[1],"this is parent process write",30);

		int status;
		wait(&status);//父进程等待子进程结束  / waitpid(-1,&status) -1 代表任何子进程 参见waitpid参数
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
