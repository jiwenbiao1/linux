/*
 * buffer.c
 *
 *  Created on: 2015年12月27日
 *      Author: biao
 */


#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <unistd.h>
#include "Buffer.h"

#define CAP_BUF_SIZE 30
#define msleep(x) usleep(x*1000)
#define INIT_NUM 0
#define TOTAL_NUM CAP_BUF_SIZE

#define READ_TYPE 0
#define WRITE_TYPE 1

sem_t p_sem,c_sem,sh_sem;
struct QUEUE{
	capture_buffer capture_buf[CAP_BUF_SIZE];
	int queue_num;
};

struct QUEUE capture_buf_queue;
//循环队列
int write_index = -1;//入队
int read_index  = 0;//出队


void SectionFunction(int type)
{
	switch(type)
	{
		case WRITE_TYPE:
				{
					//if(capture_buf_queue.queue_num < CAP_BUF_SIZE-1)
					//	{
							//可以入队列
							capture_buf_queue.queue_num++;
							write_index=(write_index+1)%CAP_BUF_SIZE;
					//	}

					break;
				}

		case READ_TYPE:
				{
					capture_buf_queue.queue_num--;
					read_index=(read_index+1)%CAP_BUF_SIZE;
					break;
				}
		default:
					break;
	}
}
void write_capture_buffer()
{
	sleep(rand()%1);
	char temp[50]={'\0'};
	int i;
	for (i = 0; i < 10; ++i) {
		temp[i]=rand()%26+65;
	}
	strcpy(capture_buf_queue.capture_buf[write_index].buf,temp);
	printf("writebuffer\t=%d\tcontent %s\n",write_index,capture_buf_queue.capture_buf[write_index].buf);
}

void read_capture_buffer()
{
	sleep(rand()%5);
	printf("readbuffer\t=%d\tcontent %s\n",read_index,capture_buf_queue.capture_buf[read_index].buf);
}

void* product(void* arg)
{
	while(1)
	{
		sem_wait(&p_sem);

			sem_wait(&sh_sem);//可以同时读写
			SectionFunction(WRITE_TYPE);
			sem_post(&sh_sem);

		write_capture_buffer();
		sem_post(&c_sem);
	}

}
void* consume(void* arg)
{
	while(1)
	{
		sem_wait(&c_sem);
			read_capture_buffer();
			sem_wait(&sh_sem);
			SectionFunction(READ_TYPE);
			sem_post(&sh_sem);
		sem_post(&p_sem);
	}

}

/*
 * 	1 sem_t 定义信号量
 * 	2 sem_init(&p_sem,0,num); 初始化信号量  第二个参数为0
 * 	3 sem_wait(&p_sem); 当信号为0时等待信号， 知道信号大于0时候才减去1  自动减1
 * 	4 sem_post(&p_sem); 信号量加1 当有线程在这个信号上面阻塞时候  激活线程
 * 	5 sem_destroy(&p_sem);清理信号量
 *
 * 	其实线程临界区可以使用信号量来实现，将信号量的信号初始化为1，然后在临界区使用完毕后再置信号量为1我们就可以轻松实现mutex了
 **/
int main(int argc, char **argv) {

	capture_buf_queue.queue_num=0;
	pthread_t ptid1,ptid2;
	sem_init(&p_sem,0,TOTAL_NUM-INIT_NUM);
	sem_init(&c_sem,0,INIT_NUM);
	sem_init(&sh_sem,0,1);//用于互斥  初始值 1
	pthread_create(&ptid1,NULL,product,NULL);
	pthread_create(&ptid2,NULL,consume,NULL);

	pthread_join(ptid1,NULL);
	pthread_join(ptid2,NULL);
	return EXIT_SUCCESS;
}
