#ifndef __RTSP_RESPONSE_H__
#define __RTSP_RESPONSE_H__

#include "Rtsp.h"
#define BOOL bool
#define LONGLONG long long
#define PCSTR const char*
#define UINT unsigned int
#define INT int
#include <sys/socket.h>   //connect,send,recv,setsockoptµÈ
#include <sys/types.h>      

#include <netinet/in.h>     // sockaddr_in, "man 7 ip" ,htons

enum RequestType
{
	requestOptions		= 0x001,
	requestDescribe		= 0x002,
	requestAnnounce		= 0x004,
	requestSetup		= 0x008,
	requestGetParameter = 0x010,
	requestSetParameter = 0x020,
	requestTeardown		= 0x040,
	requestPlay			= 0x080,
	requestPause		= 0x100,
	requestRecord       = 0x200
};



class RtspResponse : public Rtsp
{
public:
	RtspResponse(int s, sockaddr_in bindAddr, sockaddr_in connectAddr);
	virtual ~RtspResponse();

	virtual void ResponseOptions();
	virtual void ResponseDescribe(PCSTR sdp, UINT sdpLength);
	virtual void ResponseSetup(	PCSTR serverIp, INT serverRtpPort,
								PCSTR targetIp, INT targetRtpPort,
								INT32 ssrc);
	virtual void ResponseTeardown();
	virtual void ResponsePlay(PCSTR setupUrl);
	virtual void ResponsePause();
	virtual void ResponseGetParameter();
	virtual void ResponseError(PCSTR error);
	
	virtual BOOL SendResponse(PCSTR responseType);
	virtual BOOL SearchRequests(string* pStr, string field);

	virtual BOOL GetRequests();
	virtual BOOL GetRequestType(INT* pRequestType);
	virtual BOOL GetRequestMrl(string* pMrl);
	virtual BOOL GetRequestServerUrl(string* pUrl);
	virtual BOOL GetRequestSessionName(string* pSessionName);
	virtual BOOL GetRequestStreamName(string* pStreamName);
	virtual BOOL GetRequestTransportInfo(INT* pRtpPort, INT* pRtcpPort);

protected:
	LONGLONG GenerateOneNumber();

	vector<string> m_Requests;
};

#endif 