/*
 * pthread_demo.c
 *
 *  Created on: 2015年12月17日
 *      Author: biao
 */
#include "pthread_demo.h"
#include <stdio.h>
#include <unistd.h>
volatile int counter = 0;
/*
 * mutex cond 互斥变量和同步变量
 * 1 	pthread_mutex_t
 * 	 	pthread_cond_t
 * 	2 	pthread_mutex_init(&mutex, NULL);
		pthread_cond_init(&cond, NULL);
 * 	3	(1) 		pthread_mutex_lock(&mutex);//互斥锁
 * 					pthread_mutex_unlock(&mutex);
 * 		(2)			pthread_cond_wait(&cond)//同步等待信号
 * 					pthread_cond_signal(&cond)// 发出同步信号
 *
 * 4	pthread_mutex_destory(&mutex)
 * 	`	pthread_cond_destory(&mutex)
 *
 * */
pthread_mutex_t mutex;//互斥锁
pthread_cond_t cond;	// 同步锁
pthread_cond_t cond2;	// 同步锁
void delay(int sed) {
	int i, j;
	for (i = 0; i < sed; ++i) {
		for (j = 0; j < sed; ++j) {
			;
		}
	}
}
void pthread_function() {
	printf("pthread function pid = \t%d\n", getpid());
}
void pthread_function_de() {

	while (1) {
		pthread_mutex_lock(&mutex);//互斥锁
		while (counter == 0)
			pthread_cond_wait(&cond, &mutex);//等待  同步

		counter--;
		if (counter <= 5)
			pthread_cond_signal(&cond2);
		printf("decounter\t%d\n", counter);
		pthread_mutex_unlock(&mutex);
		delay(rand() % 1000);
	}

}
void pthread_function_in() {

	while (1) {
		pthread_mutex_lock(&mutex);
		counter++;
		printf("incounter\t%d\n", counter);
		if (counter >= 0)
			pthread_cond_signal(&cond);
		while (counter == 5)
			pthread_cond_wait(&cond2, &mutex);//等待  同步
		pthread_mutex_unlock(&mutex);

		delay(rand() % 1000);

	}

}
