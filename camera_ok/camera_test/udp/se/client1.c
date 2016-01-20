#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int udp_hole()
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
    sa.sin_addr.s_addr = inet_addr("104.224.166.243");

    char *p = "a";

    ssize_t size = sendto(fd, p, 1, 0, (struct sockaddr *)&sa, sizeof(sa));
    if( size < 0 ) 
    {
        printf("sendto error: %s\n", strerror(errno));
        return 1;
    }
    printf("send ok recv now\n");
    char buff[100];
    struct sockaddr_in ca;
    socklen_t slen = sizeof(struct sockaddr_in);
    memset(&ca, 0, sizeof(ca));
    size = recvfrom(fd, buff, sizeof(buff), 0, (struct sockaddr *)&ca, &slen);
    if( size < 0 ) 
    {
        printf("recvfrom error: %s\n", strerror(errno));
        return 1;
    }
    struct sockaddr_in *ps = (struct sockaddr_in *)buff;

    printf("recv another client ip: %s, port: %d\n", inet_ntoa(ps->sin_addr), ntohs(ps->sin_port));

    int i;
    for( i = 0; i < 3; i++ )
    {
        printf("send %d times\n", i);
        p = "hello world";
        ps->sin_family = AF_INET;
        size = sendto(fd, p, strlen(p), 0, (struct sockaddr *)ps, sizeof(struct sockaddr_in));
        if( size < 0 ) 
        {
            printf("sendto error: %s\n", strerror(errno));
            return 1;
        }

        printf("send %d times ok, recv now\n", i);

        char buf[100] = {0};
        struct sockaddr_in ca1;
        memset(&ca1, 0, sizeof(ca1));
        size = recvfrom(fd, buf, sizeof(buf), 0, (struct sockaddr *)&ca1, &slen);
        if( size < 0 ) 
        {
            printf("recvfrom error: %s\n", strerror(errno));
            return 1;
        }

        printf("recv %d message: %s\n", i, buf);
        sleep(1);
    }

    return 0;
}
