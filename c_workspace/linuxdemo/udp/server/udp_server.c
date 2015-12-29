/*
 * udp_client.c
 *
 *  Created on: 2015Äê12ÔÂ28ÈÕ
 *      Author: biao
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <pthread.h>
#include <netinet/in.h>
#include "udp_server.h"
int main(int argc, char **argv) {

	struct sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(SERVER_PORT);
	int sock;
	int ret;
	if(	(sock  = socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		perror("socket");

	}
	if((ret = bind(sock,(struct sockaddr*)&addr,sizeof(addr)))<0)
	{
		perror("bind");
	}

	struct sockaddr_in clientAddr;
	int len = sizeof(clientAddr);
	char recebuf[30]={'\0'};
	int length = sizeof(recebuf)/sizeof(char);

	int n;
	while(1)
	{
		if((n = recvfrom(sock,recebuf,30,0,(struct sockaddr*)&clientAddr,&len))>0 )
		{
			printf("receive package %d %s\n",length,recebuf);
			memset(recebuf,0,length);
		}
		//sendto(sock,sendbuf,length,0,(struct sockaddr* )&addr,sizeof(addr));


	}
}

