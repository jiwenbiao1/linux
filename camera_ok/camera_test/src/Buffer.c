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


sem_t p_sem,c_sem,sh_sem;

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
					//出队列操作
					capture_buf_queue.queue_num--;
					read_index=(read_index+1)%CAP_BUF_SIZE;
					break;
				}
		default:
					break;
	}
}
void write_capture_buffer(void* start,unsigned int length)
{
	
	capture_buf_queue.capture_buf[write_index].length = length;
	memcpy(capture_buf_queue.capture_buf[write_index].buf,start,length);
	printf("writebuffer\t= %d buffer_length = %d\t\n",write_index,length);
}

void read_capture_buffer(void** out,unsigned int* length)
{
		//sleep(rand()%2);
	 *length = capture_buf_queue.capture_buf[read_index].length;
	 	//不需要拷贝 直接在缓冲里面解码
	 //printf("before readbuffer\t=%d buffer_length = %d\t\n",read_index,*length);
	 *out=capture_buf_queue.capture_buf[read_index].buf;
//	 memcpy(out,capture_buf_queue.capture_buf[read_index].buf,*length);
	printf("readbuffer\t=%d buffer_length = %d\t\n",read_index,*length);
}

void* product(void* start,unsigned int length)
{
	
		sem_wait(&p_sem);

			sem_wait(&sh_sem);//可以同时读写
			SectionFunction(WRITE_TYPE);
			sem_post(&sh_sem);

		write_capture_buffer( start, length);
		sem_post(&c_sem);
	

}
void* consume(void** out,unsigned int* length)
{
	
		sem_wait(&c_sem);
			read_capture_buffer(out,length);
				sem_wait(&sh_sem);
				SectionFunction(READ_TYPE);
				sem_post(&sh_sem);
		sem_post(&p_sem);
	

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

/*
int main_test(int argc, char **argv) {

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
}*/

