#ifndef __RTSP_SESSION_H__
#define __RTSP_SESSION_H__

#include <pthread.h>
//#include "MediaStreamTransport.h"
#include "MediaStream.h"
#include "RtspResponse.h"
#include "Rtp.h"
#define SOCKET int
#define SOCKADDR_IN sockaddr_in
#define BOOL bool
#define UINT unsigned int
#define HANDLE int
#define LONGLONG long long
#define TRUE 1
#define FALSE 0
#define PSTR char*

class RtspSession
{
public:
//	RtspSession(RtspResponse* rtspResponse, MediaSessionList* mediaSessionList);
	
	RtspSession(SOCKET s, SOCKADDR_IN bindAddr, SOCKADDR_IN connectAddr);//, MediaSessionList* mediaSessionList);

	virtual ~RtspSession();

	virtual BOOL RtspSessionIsClose();

protected:
	static void* StartRtspSessionThread(void* pParam);
	void RtspSessionThread();

	void ResponseGetParameter();
	void ResponseOptions();
	void ResponseDescribe();
	void ResponseSetup();
	void ResponsePlay();
	void ResponsePause();
	void ResponseTeardown();

	BOOL TransportDescribe(string* describe);
	BOOL TransportSetup(string* pBindIp, INT* pBindPort, 
						string* pTargetIp, INT* pTargetPort, 
						INT32* pSsrc);
	BOOL TransportPlay();
	BOOL TransportPause();
	BOOL TransportTeardown();

	BOOL StreamSetup(MediaStream* pMediaStream, string* pBindIp, INT* pBindPort, string* pTargetIp, INT* pTargetPort);

	LONGLONG GenerateOneNumber();
	void GetServerIp(PSTR ip);

	HANDLE m_hProcessThread;
	HANDLE m_isStopProcessThread;

	RtspResponse*		m_pRtspResponse;
public:
	vector<MediaStream*> m_MediaStreams;
	MediaStream* RtspGetMediaStream(UINT streamIdx);
	UINT GetMediaStreamCount();
	string RtspGenerateMediaSdp(PCSTR localIpAddr, PCSTR targetIpAddr, BOOL bUseRTSP = FALSE);

	string	m_SessionName;
	string	m_SetupUrl;

	int Rtp_Port_Off;
};

#endif //__RTSP_SESSION_H__