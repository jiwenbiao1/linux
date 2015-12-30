#ifndef __SOCKET_H__
#define __SOCKET_H__

//#include <winsock2.h>
//#pragma comment(lib, "ws2_32.lib")

//#pragma warning(disable : 4786) 


#include <errno.h>
#include <vector>

// select mode 
#define SELECT_MODE_READY   0x001
#define SELECT_MODE_WRITE	0x002

// select return codes 
#define SELECT_STATE_READY         0
#define SELECT_STATE_ERROR         1
#define SELECT_STATE_ABORTED       2
#define SELECT_STATE_TIMEOUT       3
#define BOOL bool
#define UINT unsigned int
#define TRUE 1
#define FALSE 0
#define BYTE unsigned char
#define PBYTE unsigned char*
#define WORD unsigned short

#include <sys/socket.h>   //connect,send,recv,setsockoptµÈ
#include <sys/types.h>      

#include <netinet/in.h>     // sockaddr_in, "man 7 ip" ,htons
#include <poll.h>             //poll,pollfd
#include <arpa/inet.h>   //inet_addr,inet_aton
#include <unistd.h>        //read,write
#include <netdb.h>         //gethostbyname

#include <error.h>         //perror
#include <stdio.h>
#include <errno.h>         //errno
#include <string>
#include <iostream>
#include<string>
using namespace std;
class Socket
{
public:
	Socket(UINT mtu = 1500);
	virtual ~Socket();

	virtual void Close();

	virtual int Write(PBYTE pBuffer, int writeSize, UINT nTimeOut = 500000);  // 0.5sec
	virtual int Read(BYTE* pBuffer, int readSize, UINT nTimeOut = 500000); // 0.5sec

	virtual sockaddr_in GetBindAddr();
	virtual sockaddr_in GetConnectAddr();

	virtual	UINT GetMTU();

	static BOOL GetLocalIPList(vector<string>& vIPList);
	static BOOL GetAdapterSpeed(vector<int>& vList);

protected:
	void	ReportError();
	int		Select(int mode, int timeoutUsec);

	BOOL	m_isOpen;

	int		m_Socket;
	sockaddr_in m_BindAddr;
	sockaddr_in m_ConnectAddr;

	UINT	m_Mtu;
};

#endif //__SOCKET_H__