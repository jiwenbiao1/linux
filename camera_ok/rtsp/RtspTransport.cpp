#include "stdafx.h"
#include "RtspTransport.h"
#include <pthread.h>
#include<fcntl.h>
#define ULONG unsigned long

RtspTransport::RtspTransport()
{
	m_isOpen = FALSE;
	m_hListenThread = NULL;
	m_isStopListenThread = NULL;

	m_BindPort = 0;
	m_MaxConnects = 0;
}

RtspTransport::~RtspTransport(void)
{
	Close();
}

BOOL RtspTransport::Open(PCSTR bindIp, UINT bindPort, UINT maxConnects)
{
	if (m_isOpen)
		return TRUE;

	m_BindIp = bindIp;
	m_BindPort = bindPort;
	m_MaxConnects = maxConnects;

	if (m_Socket)
		close(m_Socket);
	m_Socket = socket(AF_INET, SOCK_STREAM, 0);//IPPROTO_TCP);  
	if (m_Socket == -1) 
	{
		printf("failed to create socket");
		Close();
		return FALSE;
	}

////////////////////trj changed for linux////////////////////////
	int flags = fcntl(m_Socket, F_GETFL, 0);/////trj////////////////////

	int error = fcntl(m_Socket, F_SETFL,flags | O_NONBLOCK);

	if (error == SOCKET_ERROR)
	{
//		ReportError();
		return FALSE;
	}

	m_BindAddr.sin_family = AF_INET;
	m_BindAddr.sin_port = htons(m_BindPort);
	m_BindAddr.sin_addr.s_addr = inet_addr("127.0.0.1");//m_BindIp.c_str());

	if (bind(m_Socket, (sockaddr*)&m_BindAddr, sizeof(m_BindAddr)) < 0)
	{
		printf("bind error. WSAGetLastError() = %d\n", errno);
		Close();
		return FALSE;
	}

	if (listen(m_Socket, SOMAXCONN) < 0)
	{
		printf("listen error. WSAGetLastError() = %d\n", errno);
		Close();
		return FALSE;
	}

	begin_thread(StartListenThread,this);

	m_isOpen = TRUE;
	return TRUE;
}

BOOL RtspTransport::Close()
{
	if (!m_isOpen)
		return TRUE;

	m_hListenThread = NULL;
	m_isStopListenThread = NULL;

	for(UINT32 _iPtr = 0; _iPtr < m_RtspSessionList.size(); _iPtr++) {
		delete m_RtspSessionList[_iPtr]; 
	}
	m_RtspSessionList.clear();

	if (m_Socket)
		close(m_Socket);
	m_Socket = NULL;

	m_isOpen = FALSE;

	printf("\nRtspTransport:	listen thread is closed!\n");

	return TRUE;
}

void* RtspTransport::StartListenThread(void* param)
{
	RtspTransport* pThis = (RtspTransport*)param;
	pThis->ListenThread();
	return 0;
}

void RtspTransport::ListenThread()
{
	SOCKET		connect;
	SOCKADDR_IN connectAddr;
	int			addrSize = sizeof(connectAddr);
	int			wait;
	UINT		i = 0;

	while(TRUE)
	{
		
		// delete the disconnected session///trj
		for( i = 0; i < m_RtspSessionList.size(); i ++)
		{
			if (m_RtspSessionList[i]->RtspSessionIsClose() )
			{
				delete m_RtspSessionList[i];
				m_RtspSessionList.erase(m_RtspSessionList.begin()+i);
				i--;
				printf("\nRtspTransport:	Connect session is closed!\n");
			}
		}		
		
		// create new session////trj
		socklen_t tmpLen = (socklen_t)addrSize;
		connect = accept(m_Socket, (struct sockaddr*)&connectAddr, &tmpLen);

		if(connect == SOCKET_ERROR || m_RtspSessionList.size() >= m_MaxConnects)
			continue;

		RtspSession* rtspSession = new RtspSession(	connect, m_BindAddr, connectAddr);
		m_RtspSessionList.push_back( rtspSession );
		
		printf("\nRtspTransport:	New connect session add.\n");
	}
}
