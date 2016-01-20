#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <malloc.h>

#define SERVER_PORT 8003
#define CLIENT_PORT1 8004
#define CLIENT_PORT2 8005

#define MAX_USER 20

#define COMMANDMAX 256
#define MAXRETRY 2 

#define LOGIN 1
#define LOGOUT 2
#define P2PTRANS 3
#define GETALLUSER 4

#define P2PMESSAGE 100
#define P2PMESSAGEACK 101
#define P2PSOMEONEWANTTOCALLYOU 102
#define P2PTRASH 103

struct loginmessage
{
    char username[20];
    char password[20];
};

struct logoutmessage
{
    char username[20];
};

struct p2ptranslate
{
    char username[20];
};

struct message
{
    int messagetype;
    union _message
    {
        struct loginmessage loginmember;
        struct logoutmessage logoutmember;
        struct p2ptranslate translatemessage;
    }_message;
};

struct userlistnode
{
    char username[20];
    unsigned int ip;
    unsigned short port;
};

struct p2pmessage
{
    int messagetype;
    int stringlen;
    unsigned short port;
};
