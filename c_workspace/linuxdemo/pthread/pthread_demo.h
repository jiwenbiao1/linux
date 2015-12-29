/*
 * pthread_demo.h
 *
 *  Created on: 2015Äê12ÔÂ17ÈÕ
 *      Author: biao
 */

#ifndef PTHREAD_DEMO_H_
#define PTHREAD_DEMO_H_

#include <pthread.h>
void  pthread_function();
void pthread_function_de();
void pthread_function_in();

extern volatile int  counter ;
extern pthread_mutex_t mutex;
extern pthread_cond_t  cond;
extern pthread_cond_t  cond2;
#endif /* PTHREAD_DEMO_H_ */
