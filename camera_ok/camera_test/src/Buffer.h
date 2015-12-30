
#ifndef _BUFFER_H
#define _BUFFER_H
#define YUV_FRAME_SIZE 320*240*5
typedef unsigned char uint8_t;

typedef struct   {
	uint8_t buf[YUV_FRAME_SIZE];
	int length;
}capture_buffer;

#define CAP_BUF_SIZE 30
#define msleep(x) usleep(x*1000)
#define INIT_NUM 0
#define TOTAL_NUM CAP_BUF_SIZE

#define READ_TYPE 0
#define WRITE_TYPE 1


struct QUEUE{
	capture_buffer capture_buf[CAP_BUF_SIZE];
	int queue_num;
};

extern sem_t p_sem,c_sem,sh_sem;
extern struct QUEUE capture_buf_queue;
//循环队列
extern int write_index ;//入队
extern int read_index  ;//出队

void* product(void* start,unsigned int length);

void* consume(void** out,unsigned int* length);




#endif
