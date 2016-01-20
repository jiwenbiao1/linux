#include "proto.h"

struct userlistnode userlist[MAX_USER];
int recvedack;
int sockfd;
char username[20];
char serverip[20];

void init_socket(void)
{
    struct sockaddr_in clientaddr;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("socket");
        exit(1);
    }

    bzero(&clientaddr, sizeof(clientaddr));
    clientaddr.sin_family = AF_INET;
    clientaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clientaddr.sin_port = htons(CLIENT_PORT1);
    
    if (bind(sockfd, (struct sockaddr *)&clientaddr, sizeof(clientaddr)) == -1)
    {
        perror("bind");
        exit(1);
    }
}

void connecttoserver(int sockfd, char *username, char *serverip)
{
    struct sockaddr_in servaddr;
    int usercount;
    fd_set readfds;
    fd_set writefds;
    int maxfd;
    int ret;
    int nread;
    struct timeval tv;
    int fromlen;
    int i;

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(serverip);
    servaddr.sin_port = htons(SERVER_PORT);

    struct message sendbuf;
    sendbuf.messagetype = LOGIN;
    strncpy(sendbuf._message.loginmember.username, username, strlen(username));

    sendto(sockfd, (const char *)&sendbuf, sizeof(sendbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

    fromlen = sizeof(servaddr);
    
    while (1)
    {
        FD_ZERO(&readfds);
        FD_ZERO(&writefds);
        FD_SET(sockfd, &readfds);

        int maxfd = socket;
        tv.tv_sec = 5;
        tv.tv_usec = 0;
        ret = select(maxfd + 1, &readfds, &writefds, NULL, &tv);
        printf("%d fuck\n", ret);
  
        if (ret > 0)
        {
            if (FD_ISSET(sockfd, &readfds))
            {
                nread = recvfrom(sockfd,(char *)&usercount,sizeof(int),0,(struct sockaddr *)&servaddr, &fromlen); 
                if (nread <= 0)
                {
                    perror("recvfrom");
                    exit(1);
                }
                break;
            }
        }
        else if (ret < 0)
        {
            perror("login");
            exit(1);
        }
        sendto(sockfd, (const char *)&sendbuf,sizeof(sendbuf),0,(struct sockaddr *)&servaddr, sizeof(servaddr));
    }
 
    printf("\n%d users have logined server:\n", usercount);
    printf("\n");

    for (i = 0; i < usercount; i++)
    {
        struct userlistnode *node = (struct userlistnode *)malloc(sizeof(struct userlistnode));
        if (node == NULL)
        {
            perror("malloc");
        }
        nread = recvfrom (sockfd, (char *)node, sizeof(struct userlistnode), 0, (struct sockaddr *)&servaddr,
                          &fromlen);
        if (nread < 0)
            perror("recvfrom");
        userlist[i] = *node;
        printf("%d:\n", i+1);
        printf("username: %s\n", node->username);
        struct in_addr tmp;
        tmp.s_addr = ntohl(node->ip);
        printf("userip: %s\n", inet_ntoa(tmp)); 
        printf("userport: %d\n", node->port);
        printf("\n");
    }
} 

void *recvthread(void)
{
    struct sockaddr_in remote;
    int clilen = sizeof(remote);
    struct p2pmessage recvbuf;
    int nread;
    int i;

    while (1)
    {
        nread = recvfrom(sockfd, (char *)&recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&remote, &clilen);
        if (nread < 0)
        {
            perror("recvfrom");
            continue;
        }
        
        switch (recvbuf.messagetype)
        {
            case P2PMESSAGE:
            {
                char comemessage[recvbuf.stringlen];
                nread = recvfrom(sockfd, comemessage, 256, 0, (struct sockaddr *)&remote, &clilen);
                comemessage[nread-1] = '\0';
                if (nread <= 0)
                {
                    perror("recvfrom");
                }
                else
                {
                    printf("recv a message from %s:%d -> %s",inet_ntoa(remote.sin_addr),htons(remote.sin_port),
                           comemessage);
                    struct p2pmessage sendbuf;
                    sendbuf.messagetype = P2PMESSAGEACK;
                    sendto(sockfd, (const char *)&sendbuf, sizeof(sendbuf), 0, (struct sockaddr *)&remote, 
                           sizeof(remote));
                    printf("send a message ack to %s:%d\n", inet_ntoa(remote.sin_addr), 
                           htons(remote.sin_port));
                }   
                break;           
            }
            case P2PSOMEONEWANTTOCALLYOU:
            {
                printf("recv p2psomeonewanttocallyou from %s:%d\n", inet_ntoa(remote.sin_addr),
                       htons(remote.sin_port));
       
                struct sockaddr_in remote;
                remote.sin_family = AF_INET;
                remote.sin_addr.s_addr = htonl(recvbuf.stringlen);
                remote.sin_port = htons(recvbuf.port);

                struct p2pmessage message;
                message.messagetype = P2PTRASH;
                sendto(sockfd, (const char *)&message, sizeof(message), 0, (struct sockaddr *)&remote,
                       sizeof(remote));
                printf("send p2ptrash to %s:%d\n", inet_ntoa(remote.sin_addr), htons(remote.sin_port));
                break;
            }
            case P2PMESSAGEACK:
            {
                recvedack = 1;
                printf("recv message ack from %s:%d\n", inet_ntoa(remote.sin_addr), htons(remote.sin_port));
                break; 
            }
            case P2PTRASH:
            {
                printf("recv p2ptrash data from %s:%d\n", inet_ntoa(remote.sin_addr),
                       htons(remote.sin_port));
                break;
            }
            case GETALLUSER:
            {
                int usercount;
                int fromlen = sizeof(remote);
                int nread = recvfrom(sockfd, (char *)&usercount, sizeof(int), 0, (struct sockaddr *)&remote, 
                                     &fromlen);
                if (nread <= 0)
                {
                    perror("recvfrom");
                }

                printf("%d users have logined server:\n", usercount);

                for (i = 0; i < usercount; i++)
                {
                    struct userlistnode *node = (struct userlistnode *)malloc(sizeof(struct userlistnode));
                    recvfrom(sockfd, (char *)node, sizeof(struct userlistnode), 0,
                             (struct sockaddr *)&remote, &fromlen);
                    userlist[i] = *node;
                    printf("username: %s\n", node->username);
                    struct in_addr tmp;
                    tmp.s_addr = htonl(node->ip);
                    printf("userip: %s\n", inet_ntoa(tmp));
                    printf("userport: %d\n", node->port);
                }
                break;
            }
        }
    } 
    pthread_exit(NULL);
}

void outputusage(void)
{
    printf("You can input command:\n"
           "Command type:\"send\", \"tell\", \"getu\", \"exit\"\n"
           "Example: send username message\n"
           "         tell username ip:port message\n"
           "         getu\n"
           "         exit\n");
}

int sendmessageto(char *username, char *message)
{
    char realmessage[256];
    unsigned int userip;
    unsigned short userport;
    int finduser = 0;
    int i, j;
 
    for (i = 0; i < MAX_USER; i++)
    { 
        if (strcmp(userlist[i].username, username) == 0)
        {
            userip = userlist[i].ip;
            userport = userlist[i].port;
            finduser = 1;
        }
    }
    if (!finduser)
    {
        return 0;
    }
    
    strcpy(realmessage, message);
    for (i = 0; i < MAXRETRY; i++)
    {
        recvedack = 0;
        int nsend;
        struct sockaddr_in remote;
        remote.sin_family = AF_INET;
        remote.sin_addr.s_addr = htonl(userip);
        remote.sin_port = htons(userport);
        struct p2pmessage messagehead;
        messagehead.messagetype = P2PMESSAGE;
        messagehead.stringlen = (int)strlen(realmessage) + 1;
        nsend = sendto(sockfd, (const char *)&messagehead, sizeof(messagehead), 0, 
                       (struct sockaddr *)&remote, sizeof(remote));
        nsend = sendto(sockfd, (const char *)&realmessage, messagehead.stringlen, 0,
                       (struct sockaddr *)&remote, sizeof(remote));
        for (j = 0; j < 5; j++)
        {
            if (recvedack)
                return 1;
            else
                sleep(1);
        }

        struct sockaddr_in serveraddr;
        serveraddr.sin_family = AF_INET;
        if (inet_pton(AF_INET, serverip, (struct in_addr *)&serveraddr.sin_addr.s_addr) == 0)
        {
            printf("%s error\n", serverip);
            exit(1);
        }
        serveraddr.sin_port = htons(SERVER_PORT);
        struct message transmessage;
        transmessage.messagetype = P2PTRANS;
        strcpy(transmessage._message.translatemessage.username, username);
        sendto(sockfd, (const char *)&transmessage, sizeof(transmessage), 0, (struct sockaddr *)&serveraddr,
               sizeof(serveraddr));
        sleep(20);
    }    
    return 1;
}

int sendmessageto2(char *username, char *message, const char *rip, unsigned short rport)
{
    char realmessage[256];
    unsigned int userip = 0;
    unsigned short userport = 0;
    int i, j;

    if (rip != NULL)
    {
        userip = ntohl(inet_addr(rip));
        userport = rport;
    }
    else
    {
        int finduser = 0;
        for (i = 0; i < MAX_USER; i++)
        {
            if (strcmp(userlist[i].username, username) == 0)
            {
                userip = userlist[i].username;
                userport = userlist[i].port;
                finduser = 1;
            }
        }
        if (!finduser)
            return 0;
    }

    strcpy(realmessage, message);
    struct sockaddr_in remote;
    remote.sin_family = AF_INET;
    remote.sin_addr.s_addr = htonl(userip);
    remote.sin_port = htons(userport);
    struct p2pmessage messagehead;
    messagehead.messagetype = P2PMESSAGE;
    messagehead.stringlen = (int)strlen(realmessage) + 1;
    printf("send message to %s:%d -> %s\n", inet_ntoa(remote.sin_addr), ntohs(remote.sin_port), realmessage);
   
    for (i = 0; i < MAXRETRY; i++)
    {
        recvedack = 0;
        int nsend = sendto(sockfd, (const char *)&messagehead, sizeof(messagehead), 0, 
                           (struct sockaddr *)&remote, sizeof(remote));
        nsend = sendto(sockfd, (const char *)&realmessage, messagehead.stringlen, 0,
                       (struct sockaddr *)&remote, sizeof(remote));
        for (j = 0; j < 10; j++)
        {
            if (recvedack)
                return 1;
            else 
                sleep(300);
        }
    }
    return 0;
}

void parsecommand(char *commandline)
{
    if (strlen(commandline) < 4)
        return;
    char command[10];
    strncpy(command, commandline, 4);
    command[4] = '\0';

    if (strcmp(command, "exit") == 0)
    {
        struct message sendbuf;
        sendbuf.messagetype = LOGOUT;
        strncpy(sendbuf._message.logoutmember.username, username, 20);
        struct sockaddr_in serveraddr;
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = inet_addr(serverip);
        serveraddr.sin_port = htons(SERVER_PORT);
        sendto(sockfd, (const char *)&sendbuf, sizeof(sendbuf), 0, (struct sockaddr *)&serveraddr, 
               sizeof(serveraddr));
        shutdown(sockfd, SHUT_RDWR);
        close(sockfd);
        exit(0);
    }
    else if (strcmp(command, "send") == 0)
    {
        char sendname[20];
        char message[COMMANDMAX];
        int i;
        
        for (i = 5; ; i++)
        {
            if (commandline[i] != ' ')
                sendname[i-5] = commandline[i];
            else 
            { 
                sendname[i-5] = '\0';
                break;
            }
        }
        strcpy(message, &(commandline[i+1]));
        if(sendmessageto(sendname, message))
            printf("send ok!\n");
        else
            printf("send failure!\n");
    }
    else if (strcmp(command, "tell") == 0)
    {
        char sendname[20];
        char sendto[64] = {0};
        char message[COMMANDMAX];
        int i;

        for (i = 5; ; i++)
        {
            if (commandline[i] != ' ')
                sendname[i-5] = commandline[i];
            else
            {
                sendname[i-5] = '\0';
                break;
            }
        }
        i = i + 1;
        int nstart = i;
        for (;;i++)
        {
            if (commandline[i] != ' ')
                sendto[i-nstart] = commandline[i];
            else
            {
                sendto[i-nstart] = '\0';
                break;
            }
        }
        strcpy(message, &(commandline[i+1]));

        char ip[32] = {0};
        char *p1 = sendto;
        char *p2 = ip;
        while (*p1 != ':')
        {
            *p2++ = *p1++;
        }
        p1++;
        unsigned short port = atoi(p1);
        printf("%s\n", sendname);
        printf("%s\n", message);
        printf("%s\n", ip);
        printf("%d\n", port);
        if (sendmessageto2(sendname, message, strcmp(ip, "255.255.255.255") ? ip : NULL, port))
            printf("send ok!\n");
        else
            printf("send failure!\n");
    }
    else if (strcmp(command, "getu") == 0)
    {
        struct message sendbuf;
        sendbuf.messagetype = GETALLUSER;
        strncpy(sendbuf._message.loginmember.username, username, 20);
        struct sockaddr_in serveraddr;
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = inet_addr(serverip);
        serveraddr.sin_port = htons(SERVER_PORT);
        sendto(sockfd, (const char *)&sendbuf, sizeof(sendbuf), 0, (struct sockaddr *)&serveraddr,
               sizeof(serveraddr));

#if 0
        int commandgetu = GETALLUSER;
        struct sockaddr_in serveraddr;
        serveraddr.sin_family = AF_INET;
        serveraddr.sin_addr.s_addr = inet_addr(serverip);
        serveraddr.sin_port = htons(SERVER_PORT);
        sendto(sockfd, (const char *)&commandgetu, sizeof(commandgetu), 0, (struct sockaddr *)&serveraddr, 
               sizeof(serveraddr));
#endif
    }
}
    
int main(int argc, char **argv)
{
    pthread_t child_thread;
    pthread_attr_t child_thread_attr;
    
    init_socket();

    printf("please input server ip: ");
    scanf("%s", serverip);
    printf("please input your name: ");
    scanf("%s", username);
 
    connecttoserver(sockfd, username, serverip);

    pthread_attr_init(&child_thread_attr);
    pthread_attr_setdetachstate(&child_thread_attr, PTHREAD_CREATE_DETACHED);
    if (pthread_create(&child_thread, &child_thread_attr, recvthread, NULL) < 0)
    {
        perror("pthread_create");
    } 

    outputusage();

    while (1)
    {
        char command[COMMANDMAX];
        fgets(command, COMMANDMAX, stdin);
        parsecommand(command);
    }

    return 0;
}
