#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#define msleep(x) usleep(x*1000)
#define INIT_NUM 4
#define TOTAL_NUM 10
int lib = INIT_NUM;
sem_t p_sem,c_sem,sh_sem;

void* product(void* arg)
{
	while(1)
	{
		sem_wait(&p_sem);
			sleep(rand()%5);
			sem_wait(&sh_sem);
			lib++;
			printf("product\tlib = %d\n",lib);
			sem_post(&sh_sem);
		sem_post(&c_sem);
	}

}
void* consume(void* arg)
{
	while(1)
	{
		sem_wait(&c_sem);
			sleep(rand()%5);
			sem_wait(&sh_sem);
			lib--;
			printf("consume\tlib = %d\n",lib);
			sem_post(&sh_sem);
		sem_post(&p_sem);
	}

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
int main(int argc, char **argv) {

	pthread_t ptid1,ptid2;
	sem_init(&p_sem,0,TOTAL_NUM-INIT_NUM);
	sem_init(&c_sem,0,INIT_NUM);
	sem_init(&sh_sem,0,1);//用于互斥  初始值 1
	pthread_create(&ptid1,NULL,product,NULL);
	pthread_create(&ptid2,NULL,consume,NULL);

	pthread_join(ptid1,NULL);
	pthread_join(ptid2,NULL);
	return EXIT_SUCCESS;
}
