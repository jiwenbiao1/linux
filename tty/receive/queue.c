#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

void  LookQueue(Queue * queue)
{	int i=0;
	while(i<=MAX_BUF_SIZE)
		printf("%.2x\t",queue->buf[i++]);
	printf("\n");
}
int InitQueue(Queue* queue)
{
	queue->count = 0;
	memset(queue->buf,0,MAX_BUF_SIZE*sizeof(DataType));
	queue->front = 0;
	queue->rear= 0;
}
//满了 (rear+1) %(MAX_BUFFER_SIZE)= front
//  front指向队首元素，rear指向队尾元素的下一个元素
int InQueue(Queue* queue,DataType* buf,int num)
{
	if((queue->count + num ) <= MAX_BUF_SIZE-1)
		{
		//可以放入缓冲
			queue->count+=num;
			DataType* temp = buf;
			while(num--)
				{
					//printf("%0.2x\t%d\n",*temp,queue->rear);
					queue->buf[queue->rear] = *temp++; 
					queue->rear = (++(queue->rear))%MAX_BUF_SIZE;
				}
		}
	else 
		{
			perror("queue overfollow");
			return -1;
		}
}

int  OutQueue(Queue *queue,DataType* buf, int num)
{	
	if((queue->count - num ) >= 0)
		{
		//printf("queue->count = %d\n",queue->count);
			queue->count-=num;
			DataType* temp = buf;
			int out_count = num;
			while(out_count--)
				{
					*temp = queue->buf[queue->front];
					//printf("%0.2x\n",*temp);
					queue->front = ((queue->front)+1)%MAX_BUF_SIZE;
					temp++;
					
					
				}
			return num;
		}
	else if((queue->count - num)<0&&queue->count>0)
		{
			int out_count =queue->count ;
			DataType* temp = buf;
			while(queue->count--)
				{
					*temp = queue->buf[queue->front];
					queue->front = (queue->front++)%MAX_BUF_SIZE;
					temp++;
					
				}
			return out_count ;
		}
	else 
		{
			perror("queue empty");
			return -1;
		}	
}
