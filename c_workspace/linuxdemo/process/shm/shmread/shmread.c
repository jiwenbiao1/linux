/*
 * shm.c
 *
 *  Created on: 2015��12��19��
 *      Author: biao
 */
#include "common.h"
#define SHM_SIZE 100
int main(int argc, char **argv) {

	int shm_id = shmget((key_t)10,SHM_SIZE,0644|IPC_CREAT);
	void *p = shmat(shm_id,NULL,0);
	if(p==(void*)-1)
	{
		perror("shmat");
	}
	printf("\nget share memory %X\n",p);
	while(1)
	{
		printf("%s\n",p);
		sleep(1);
	}
	shmdt(p);
	shmctl(shm_id,IPC_RMID,(struct shmid_ds*)0);
	return EXIT_FAILURE;

}
/*
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/shm.h>
#include "shmdata.h"

int main()
{
	int running = 1;//�����Ƿ�������еı�־
	void *shm = NULL;//����Ĺ����ڴ��ԭʼ�׵�ַ
	struct shared_use_st *shared;//ָ��shm
	int shmid;//�����ڴ��ʶ��
	//���������ڴ�
	shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666|IPC_CREAT);
	if(shmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}
	printf("\nshmget id =  %ld\n", shmid);
	//�������ڴ����ӵ���ǰ���̵ĵ�ַ�ռ�
	shm = shmat(shmid, 0, 0);
	if(shm == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	printf("\nMemory attached at %X\n", (int)shm);
	//���ù����ڴ�
	shared = (struct shared_use_st*)shm;
	shared->written = 0;
	while(running)//��ȡ�����ڴ��е�����
	{
		//û�н��������ڴ涨���������ݿɶ�ȡ
		if(shared->written != 0)
		{
			//��ȡ����
			printf("You wrote: %s", shared->text);
			sleep(rand() % 3);
			//��ȡ�����ݣ�����writtenʹ�����ڴ�ο�д
			shared->written = 0;
			//������end���˳�ѭ��������
			if(strncmp(shared->text, "end", 3) == 0)
				running = 0;
		}
		else//������������д���ݣ����ܶ�ȡ����
			sleep(1);
	}
	//�ѹ����ڴ�ӵ�ǰ�����з���
	if(shmdt(shm) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	//ɾ�������ڴ�
	if(shmctl(shmid, IPC_RMID, 0) == -1)
	{
		fprintf(stderr, "shmctl(IPC_RMID) failed\n");
		exit(EXIT_FAILURE);
	}
	exit(EXIT_SUCCESS);
}*/