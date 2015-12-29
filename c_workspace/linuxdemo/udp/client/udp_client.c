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
#include "udp_client.h"
int main(int argc, char **argv) {

	struct sockaddr_in addr;
	int sock;
	if(	(sock  = socket(AF_INET,SOCK_DGRAM,0))<0)
	{
		perror("socket");
	}

	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(SERVER_IP);
	addr.sin_port = htons(SERVER_PORT);
	char sendbuf[20]={'\0'};
	int length = sizeof(sendbuf)/sizeof(char);
	while(1)
	{
//		sendfile();
		strcpy(sendbuf,"hello i am client");
		sendto(sock,sendbuf,length,0,(struct sockaddr* )&addr,sizeof(addr));
		sleep(1);
	}
}

