/*
 * main.c
 *
 *  Created on: 2015年12月17日
 *      Author: biao
 */



#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <pthread.h>
#include <sys/types.h>
#include "pthread_demo.h"



 int main_test(int argc, char **argv) {

	 /*
	  * 1 pthread 变量
	  * 2. pthrad create
	  * 3. pthread_join 同步主线程和子线程
	  * */
	 pthread_t pthread_1;
	 pthread_t pthread_2;
	 pthread_mutex_init(&mutex,NULL);
	 int ret = pthread_create(&pthread_1,NULL,(void*)pthread_function_de,NULL);
	 if (ret) {
		 perror("pthread create");
		 exit(1);

	}
	 ret = pthread_create(&pthread_2,NULL,(void*)pthread_function_in,NULL);
	 	 if (ret) {
	 		 perror("pthread create");
	 		 exit(1);

	 	}





	// sleep(1);
	 pthread_join(pthread_1,NULL);
	 pthread_join(pthread_2,NULL);
	 pthread_mutex_destroy(&mutex);
//	 printf("main process\n");
	 return 0;
}
