#ifndef __RTSP_TRANSPORT_H__
#define __RTSP_TRANSPORT_H__

#include "RtspSession.h"
#include "MediaSession.h"
#define BOOL bool
#define UINT unsigned int
#define SOCKET int
#define HANDLE int
#define SOCKADDR_IN sockaddr_in
#define PCSTR const char*

class RtspTransport
{
public:
	RtspTransport();
	
	virtual ~RtspTransport(void);

	virtual BOOL Open(PCSTR bindIp = "127.0.0.1", UINT bindPort = 554, UINT maxConnects = 10);

	virtual BOOL Close();

protected:
	static void* StartListenThread(void* param);
	void ListenThread();

	string		m_BindIp;
	UINT		m_BindPort;
	UINT		m_MaxConnects;
	SOCKET		m_Socket;
	SOCKADDR_IN m_BindAddr;

	BOOL		m_isOpen;

	HANDLE		m_hListenThread;
	HANDLE		m_isStopListenThread;
public:///trj
	vector<RtspSession*> m_RtspSessionList;
};

#endif