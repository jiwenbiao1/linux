/*
 * buffer.c
 *
 *  Created on: 2015��12��27��
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
//ѭ������
int write_index = -1;//���
int read_index  = 0;//����


void SectionFunction(int type)
{
	switch(type)
	{
		case WRITE_TYPE:
				{
					//if(capture_buf_queue.queue_num < CAP_BUF_SIZE-1)
					//	{
							//���������
							capture_buf_queue.queue_num++;
							write_index=(write_index+1)%CAP_BUF_SIZE;
					//	}

					break;
				}

		case READ_TYPE:
				{
					//�����в���
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
	 	//����Ҫ���� ֱ���ڻ����������
	 //printf("before readbuffer\t=%d buffer_length = %d\t\n",read_index,*length);
	 *out=capture_buf_queue.capture_buf[read_index].buf;
//	 memcpy(out,capture_buf_queue.capture_buf[read_index].buf,*length);
	printf("readbuffer\t=%d buffer_length = %d\t\n",read_index,*length);
}

void* product(void* start,unsigned int length)
{
	
		sem_wait(&p_sem);

			sem_wait(&sh_sem);//����ͬʱ��д
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
 * 	1 sem_t �����ź���
 * 	2 sem_init(&p_sem,0,num); ��ʼ���ź���  �ڶ�������Ϊ0
 * 	3 sem_wait(&p_sem); ���ź�Ϊ0ʱ�ȴ��źţ� ֪���źŴ���0ʱ��ż�ȥ1  �Զ���1
 * 	4 sem_post(&p_sem); �ź�����1 �����߳�������ź���������ʱ��  �����߳�
 * 	5 sem_destroy(&p_sem);�����ź���
 *
 * 	��ʵ�߳��ٽ�������ʹ���ź�����ʵ�֣����ź������źų�ʼ��Ϊ1��Ȼ�����ٽ���ʹ����Ϻ������ź���Ϊ1���ǾͿ�������ʵ��mutex��
 **/

/*
int main_test(int argc, char **argv) {

	capture_buf_queue.queue_num=0;
	pthread_t ptid1,ptid2;
	sem_init(&p_sem,0,TOTAL_NUM-INIT_NUM);
	sem_init(&c_sem,0,INIT_NUM);
	sem_init(&sh_sem,0,1);//���ڻ���  ��ʼֵ 1
	pthread_create(&ptid1,NULL,product,NULL);
	pthread_create(&ptid2,NULL,consume,NULL);

	pthread_join(ptid1,NULL);
	pthread_join(ptid2,NULL);
	return EXIT_SUCCESS;
}*/

