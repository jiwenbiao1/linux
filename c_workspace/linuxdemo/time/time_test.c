/*
 * time_test.c
 *
 *  Created on: 2015年12月26日
 *      Author: biao
 */

#include "stdio.h"
#include "stdlib.h"
#include "time.h"
/*

 int main(void)
 {
 struct tm *local;
 time_t t;
 t=time(NULL);
 printf("%ld\n",t);

 local=localtime(&t);
 printf("Local hour is: %d:%d:%d\n",local->tm_hour,local->tm_min,local->tm_sec);
 local=gmtime(&t);
 printf("UTC hour is:  %d:%d:%d\n",local->tm_hour,local->tm_min,local->tm_sec);
 return 0;
 }
 */

/*

 void delay()
 {
 int i ,j;
 for ( i = 0; i < 100000; ++i) {
 for (j = 0; j < 1000; ++j) {
 ;
 }
 }
 }
 int main(void)
 {
 time_t start,end;
 start = time(NULL);
 delay();
 end = time(NULL);
 printf("The pause used %ld seconds.\n",end-startdifftime(end,start));//<-
 sleep(1);
 return 0;
 }*/
#include <sys/time.h>

/*
gettimeofday将时间保存在结构tv之中.tz一般我们使用NULL来代替。

以下是程序：*/
#include <sys/time.h>
#include <stdio.h>
#include <math.h>
void function()
{
	unsigned int i,j;
	double y;
	for(i=0;i<1000;i++)
	for(j=0;j<1000;j++)
	y=sin((double)i);
}
main() {
	struct timeval tpstart, tpend;
	float timeuse;
	gettimeofday(&tpstart, NULL);
	function();
	gettimeofday(&tpend, NULL);
	timeuse = 1000000 * (tpend.tv_sec - tpstart.tv_sec) + tpend.tv_usec
			- tpstart.tv_usec;
	timeuse /= 1000000;
	printf("Used Time:%f\n", timeuse);
	exit(0);
}
