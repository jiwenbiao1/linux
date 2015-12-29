/*
 * shm.c
 *
 *  Created on: 2015��12��19��
 *      Author: biao
 */
#include "common.h"
#define SHM_SIZE 100
/*
 * �����ڴ�  ���ṩͬ������ ��Ҫʹ���ź���
 * http://blog.csdn.net/ljianhui/article/details/10253345
 * 		1 shmget  key ��Ҫͬ���Ľ��̹���һ��key
 * 		2 shmat  �������� ���ص�ַ
 * 		3 shmdt  �Ӵ�����
 * 		4 shmctl ɾ���ڴ�
 * */
int main(int argc, char **argv) {

	char buffer[SHM_SIZE+1]={'\0'};
	int shm_id = shmget((key_t)10,SHM_SIZE,0644|IPC_CREAT);
	void *p = shmat(shm_id,NULL,0);
	if(p==(void*)-1)
	{
		perror("shmat");
	}
	printf("\nget share memory %X\n",p);
	while(1)
	{


		fgets(buffer, SHM_SIZE, stdin);
		strcpy(p,buffer);
	}
	shmdt(p);
	shmctl(shm_id,IPC_RMID,(struct shmid_ds*)0);
	return EXIT_SUCCESS;


}













/*#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/shm.h>
#include "shmdata.h"

int main()
{
	int running = 1;
	void *shm = NULL;
	struct shared_use_st *shared = NULL;
	char buffer[BUFSIZ + 1];//���ڱ���������ı�
	int shmid;
	//���������ڴ�
	shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0666|IPC_CREAT);
	if(shmid == -1)
	{
		fprintf(stderr, "shmget failed\n");
		exit(EXIT_FAILURE);
	}
	printf("\nshmget id =  %ld\n", shmid);
	//�������ڴ����ӵ���ǰ���̵ĵ�ַ�ռ�
	shm = shmat(shmid, (void*)0, 0);
	if(shm == (void*)-1)
	{
		fprintf(stderr, "shmat failed\n");
		exit(EXIT_FAILURE);
	}
	printf("Memory attached at %X\n", (int)shm);
	//���ù����ڴ�
	shared = (struct shared_use_st*)shm;
	while(running)//�����ڴ���д����
	{
		//���ݻ�û�б���ȡ����ȴ����ݱ���ȡ,���������ڴ���д���ı�
		while(shared->written == 1)
		{
			sleep(1);
			printf("Waiting...\n");
		}
		//�����ڴ���д������
		printf("Enter some text: ");
		fgets(buffer, BUFSIZ, stdin);
		strncpy(shared->text, buffer, TEXT_SZ);
		//д�����ݣ�����writtenʹ�����ڴ�οɶ�
		shared->written = 1;
		//������end���˳�ѭ��������
		if(strncmp(buffer, "end", 3) == 0)
			running = 0;
	}
	//�ѹ����ڴ�ӵ�ǰ�����з���
	if(shmdt(shm) == -1)
	{
		fprintf(stderr, "shmdt failed\n");
		exit(EXIT_FAILURE);
	}
	sleep(2);
	exit(EXIT_SUCCESS);
}*/
