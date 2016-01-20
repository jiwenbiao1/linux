#include "proto.h"

int init_socket(void)
{
    int sockfd;
    struct sockaddr_in serveraddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(SERVER_PORT);

    if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
    {
        perror("bind");
        exit(1);
    }

    return sockfd;
}

int main(int argc, char **argv)
{
    int sockfd;

    sockfd = init_socket();

    struct sockaddr_in clientaddr;
    struct message recvbuf;

    memset(&recvbuf, 0, sizeof(struct message));

    int clilen;
    int ret;
    int messagetype;
    struct userlistnode userlist[MAX_USER] = {{0}, 0, 0};
    int i;
    int usercount;

    while (1)
    {
        clilen = sizeof(clientaddr);
        ret = recvfrom(sockfd, (char *)&recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&clientaddr, &clilen);
        if (ret <= 0)
        {
            perror("recvfrom");
            continue;
        }
        else
        {
            messagetype = recvbuf.messagetype;
            switch (messagetype)
            {
                case LOGIN:
                {
                    struct userlistnode *currentuser = (struct userlistnode *)malloc(sizeof(struct userlistnode));
                    if (currentuser == NULL)
                    {
                        perror("malloc");
                    }
                    strcpy(currentuser->username, recvbuf._message.loginmember.username);
                    currentuser->ip = ntohl(clientaddr.sin_addr.s_addr);
                    currentuser->port = ntohs(clientaddr.sin_port);
 
                    int samename = 0;
                    for (i = 0; i < MAX_USER; i++)
                    {
                        if (strcmp((userlist[i]).username, recvbuf._message.loginmember.username) == 0)
                        {
                            samename = 1;
                            break;
                        }
                    }

                    if (!samename)
                    {
                        printf("a user login:\n  username: %s\n  ip: %s\n  port: %d\n",  
                               recvbuf._message.loginmember.username, inet_ntoa(clientaddr.sin_addr), 
                               ntohs(clientaddr.sin_port));
                        for (i = 0; i < MAX_USER; i++)
                        {
                            if ((userlist[i]).username[0] == 0)
                            {
                                userlist[i] = *currentuser;
                                break;
                            }
                            else
                                continue;
                        }
                        usercount = i+1;
                    }

                    sendto(sockfd, (const char *)&usercount, sizeof(int), 0, (struct sockaddr *)&clientaddr,
                           sizeof(clientaddr));

                    for (i = 0; i < MAX_USER; i++)
                    {
                        if ((userlist[i]).username[0] != 0)   
                            sendto(sockfd, (char *)&userlist[i], sizeof(struct userlistnode), 0,
                                   (struct sockaddr *)&clientaddr, sizeof(clientaddr));
                    }

                    printf("send user list information to: %s <-> %s:%d\n",recvbuf._message.loginmember.username,
                            inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
                    break;
                }
                case LOGOUT:
                {
                    printf("a user logout:\n  username: %s\n  ip: %s\n  port: %d\n", 
                           recvbuf._message.logoutmember.username, inet_ntoa(clientaddr.sin_addr),
                           ntohs(clientaddr.sin_port));
                    for (i = 0; i < MAX_USER; i++)
                    {
                        if (strcmp((userlist[i]).username, recvbuf._message.logoutmember.username) == 0)
                        {
                            userlist[i].username[0] = 0;
                            break;
                        }
                    }
                    break;
                }
                case P2PTRANS:
                {
                    printf("%s:%d wants to p2p %s\n", inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port),
                           recvbuf._message.translatemessage.username);
                 
                    struct userlistnode node;
                    
                    for (i = 0; i < MAX_USER; i++)
                    {
                        if (!strcmp(userlist[i].username, recvbuf._message.translatemessage.username)) 
                        {
                            node = userlist[i];
                            break;
                        }
                    }

                    struct sockaddr_in remote;
                    remote.sin_family = AF_INET;
                    remote.sin_port = htons(node.port);
                    remote.sin_addr.s_addr = htonl(node.ip);

                    struct p2pmessage transmessage;
                    transmessage.messagetype = P2PSOMEONEWANTTOCALLYOU;
                    transmessage.stringlen = ntohl(clientaddr.sin_addr.s_addr);
                    transmessage.port = ntohs(clientaddr.sin_port);

                    sendto(sockfd, (const char *)&transmessage, sizeof(transmessage), 0,
                           (struct sockaddr *)&remote, sizeof(remote));
 
                    printf("tell %s < %s:%d > ",  
                            recvbuf._message.translatemessage.username, inet_ntoa(remote.sin_addr), node.port);
                    printf("to send p2ptrans message to: %s:%d\n",
                            inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));
                    break;
                }
                case GETALLUSER:
                {
                    struct p2pmessage sendbuf;
                    sendbuf.messagetype = GETALLUSER;
                    sendto(sockfd, (const char *)&sendbuf, sizeof(sendbuf), 0,
                           (struct sockaddr *)&clientaddr, sizeof(clientaddr));
                    sendto(sockfd, (const char *)&usercount, sizeof(int), 0, (struct sockaddr *)&clientaddr,
                           sizeof(clientaddr));

                    for (i = 0; i < MAX_USER; i++)
                    {
                        if ((userlist[i]).username[0] != 0)
                            sendto(sockfd, (char *)&userlist[i], sizeof(struct userlistnode), 0,
                                   (struct sockaddr *)&clientaddr, sizeof(clientaddr));
                    }

                    printf("send user list information to: %s <-> %s:%d\n",recvbuf._message.loginmember.username,
                            inet_ntoa(clientaddr.sin_addr), ntohs(clientaddr.sin_port));

                    break; 
                }
            }
        }
    }
}
