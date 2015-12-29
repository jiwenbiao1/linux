/*
 * memory.c
 *
 *  Created on: 2015Äê12ÔÂ18ÈÕ
 *      Author: biao
 */
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <sys/mman.h>
#include <fcntl.h>

void memory() {
	int *p, *q;
	printf("pagesize = %d\n", getpagesize());
	if ((p = calloc(sizeof(int), 10)) == NULL) {
		perror("calloc");
	}
	if ((q = malloc(sizeof(int) * 10)) == NULL) {
		perror("malloc");
	}

	int i;
	printf("calloc\n");
	for (i = 0; i < 10; ++i) {
		printf("%d\t", p[i]);
	}
	printf("malloc\n");
	for (i = 0; i < 10; ++i) {
		printf("%d\t", p[i]);
	}
	free(p);
	free(q);
	sleep(1);
}
void mmap_test(){
	//extern void *mmap (void *__addr, size_t __len, int __prot, int __flags, int __fd, off_t __off);
	void *p=NULL;
	int share_fd = open("a.txt",O_RDWR);
	if(share_fd==-1)
	{
		perror("open");
		return ;
	}
	p = mmap(NULL,100,PROT_WRITE|PROT_READ,MAP_SHARED,share_fd,0);
	if(p==NULL)
	{
		perror("mmap");

	}
	printf("%s",p);
	strcpy(p,"this is overwrite");
	strcpy(p+20,"this is overwrite");
	munmap(p,100);



}
int main(int argc, char **argv) {

	//memory();
	//mmap_test();
	printf("user = %s\n",getenv("USER"));
	putenv("USER=test");
	printf("user = %s\n",getenv("USER"));
	return EXIT_SUCCESS;

}

