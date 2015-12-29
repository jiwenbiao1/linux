#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <semaphore.h>
#include <unistd.h>
#include <malloc.h>
#include "video_capture.h"
#include "buffer.h"
#include <time.h>


struct camera *cam;


void capture_encode_thread(void) {
	int count = 1;
	for (;;) {
		//开始计时
		struct timeval tpstart, tpend;
		float timeuse;
		gettimeofday(&tpstart, NULL);
		
		struct tm* local;
		time_t t;
		t = time(NULL);
		local = localtime(&t);
		printf("  %d:%d:%d\t",local->tm_hour,local->tm_min,local->tm_sec);
		printf("-->this is the %dth frame\n", count);
		count++;
		/*if (count++ >= 50) // 閲囬泦100甯х殑鏁版嵁
				{
			printf("------need to exit from thread------- \n");
			break;
		}*/
		
		fd_set fds;
		struct timeval tv;
		int r;

		FD_ZERO(&fds);
		FD_SET(cam->fd, &fds);

		/* Timeout. */
		tv.tv_sec = 0;
		tv.tv_usec = 1000*200;

		r = select(cam->fd + 1, &fds, NULL, NULL, &tv);

		if (-1 == r) {
			if (EINTR == errno)
				continue;

			errno_exit("select");
		}

		if (0 == r) {
			fprintf(stderr, "select timeout\n");
			//exit(EXIT_FAILURE);
		}

		
		////////////////
		if (read_and_encode_frame(cam) != 1) {
			fprintf(stderr, "read_fram fail in thread\n");
			break;
		}
		
		//结束计时
		gettimeofday(&tpend, NULL);
		timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec - tpstart.tv_usec;
		timeuse /= 1000;//ms
		printf("\t\t\tUsed Time:%f\n", timeuse);
		
		//////////////////
		/*
		if (read_and_encode_frame(cam) != 1) {
			fprintf(stderr, "read_fram fail in thread\n");
			break;
		}*/
	}
}

int main(int argc, char **argv) {

	capture_buf_queue.queue_num=0;
	pthread_t ptid1,ptid2;
	sem_init(&p_sem,0,TOTAL_NUM-INIT_NUM);
	sem_init(&c_sem,0,INIT_NUM);
	sem_init(&sh_sem,0,1);//用于互斥  初始值 1
		
	cam = (struct camera *) malloc(sizeof(struct camera));
	if (!cam) {
		printf("malloc camera failure!\n");
		exit(1);
	}
	cam->device_name = "/dev/video2";
	cam->buffers = NULL;
	cam->width = 176;
	cam->height = 144;
	cam->display_depth = 5; /* RGB24 */


	v4l2_init(cam);

	if (0 != pthread_create(&ptid1, NULL, (void *) capture_encode_thread, NULL)) {
		fprintf(stderr, "thread capture create fail\n");
	}
	if(0!=pthread_create(&ptid2,NULL,(void*)encod_from_buffer,NULL))
	{
		fprintf(stderr,"thread encode create fial\n");
	}

	pthread_join(ptid1,NULL);
	pthread_join(ptid2,NULL);
	printf("-----------end program------------");
	v4l2_close(cam);

	return 0;
}
