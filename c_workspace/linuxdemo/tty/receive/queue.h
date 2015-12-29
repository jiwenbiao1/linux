#ifndef _QUEUE_H
#define _QUEUE_H



#define MAX_BUF_SIZE 512

#define DataType unsigned char 
typedef struct 
{
	DataType buf[MAX_BUF_SIZE];
	int front;
	int rear;
	int count;
}Queue;

int InitQueue(Queue* queue);
int InQueue(Queue* queue,DataType* buf,int num);


int  OutQueue(Queue *queue,DataType* buf, int num);
void  LookQueue(Queue * queue);


#endif
