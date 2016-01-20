#include <sys/socket.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>

int main(int argc, const char *argv[])
{
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if( fd < 0 ) 
    {
        printf("socket error: %s\n", strerror(errno));
        return 1;
    }

    struct sockaddr_in sa;
    memset(&sa, 0, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(5555);
    sa.sin_addr.s_addr = INADDR_ANY;

    int ret = bind(fd, (struct sockaddr *)&sa, sizeof(sa));
    if( ret < 0 ) 
    {
        printf("bind error: %s\n", strerror(errno));
        return 1;
    }

    struct sockaddr_in ca[2];
    memset(&ca, 0, sizeof(ca));
    char buff[100];
    socklen_t  slen = sizeof(struct sockaddr_in);
    int i = 0;
    ssize_t size;
    for(; i < 2; i++)
    {	
		printf("paper to recv\n");
        size = recvfrom(fd, &buff, sizeof(buff), 0, (struct sockaddr *)&(ca[i]), &slen);
        if( size < 0 ) 
        {
            printf("recvfrom error: %s\n", strerror(errno));
            return 1;
        }

        printf("recvfrom ip: %s, port: %d\n", inet_ntoa(ca[i].sin_addr), ntohs(ca[i].sin_port));
    }

    size = sendto(fd, &ca[0], sizeof(ca[0]), 0, (struct sockaddr *)&ca[1], sizeof(ca[1]));
    if ( size < 0 )
    {
        printf("sendto error: %s\n", strerror(errno));
        return 1;
    }

    size = sendto(fd, &ca[1], sizeof(ca[1]), 0, (struct sockaddr *)&ca[0], sizeof(ca[0]));
    if ( size < 0 )
    {
        printf("sendto error: %s\n", strerror(errno));
        return 1;
    }

    return 0;
}

