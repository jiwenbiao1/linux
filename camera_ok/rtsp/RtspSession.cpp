#include "stdafx.h"
#include "RtspSession.h"
#define LARGE_INTEGER int
#include"MediaStreamH264.h"////trj
#include"RtspService.h"
#define NETWORK_MTU 1482

RtspSession::~RtspSession()
{
	delete m_pRtspResponse;
}

void* RtspSession::StartRtspSessionThread(void* pParam)
{
	RtspSession* pThis = (RtspSession*)pParam;
	pThis->RtspSessionThread();

	return 0;
}

void RtspSession::RtspSessionThread()
{
	int requestType;
	int waite;

	while(TRUE)
	{
		if ( !m_pRtspResponse->GetRequests() )
			break;

		if ( !m_pRtspResponse->GetRequestType(&requestType) )
			continue;

		switch(requestType)
		{
		case requestOptions:
			ResponseOptions();
			break;
		case requestDescribe:
			ResponseDescribe();
			break;
		case requestSetup:
			ResponseSetup();
			break;
		case requestPlay:
			ResponsePlay();
			break;
		case requestPause:
			ResponsePause();
			break;
		case requestTeardown:
			ResponseTeardown();
			break;
		case requestGetParameter:
			ResponseGetParameter();
			break;
		default:
			break;
		}
	}
}

BOOL RtspSession::RtspSessionIsClose()
{
	return FALSE;
}

void RtspSession::ResponseOptions()
{
	m_pRtspResponse->ResponseOptions();
}

void RtspSession::ResponseDescribe()
{
	string sdp;

	if (!TransportDescribe(&sdp) )
		return;

	m_pRtspResponse->ResponseDescribe(sdp.c_str(), sdp.length());
}

void RtspSession::ResponseSetup()
{
	string	serverIp;
	INT		serverPort;
	string	targetIp;
	INT		targetPort;
	INT32	ssrc;
	if (!TransportSetup(&serverIp, &serverPort, &targetIp, &targetPort, &ssrc) )
		return;
	m_pRtspResponse->ResponseSetup(serverIp.c_str(), serverPort, targetIp.c_str(), targetPort, ssrc);
}

void RtspSession::ResponsePlay()
{
	if ( !TransportPlay() )
		return;

	m_pRtspResponse->ResponsePlay(m_SetupUrl.c_str());
}

void RtspSession::ResponsePause()
{
	if ( !TransportPause() )
		return;

	m_pRtspResponse->ResponsePause();

}

void RtspSession::ResponseTeardown()
{
	if ( !TransportTeardown() )
		return;

	m_pRtspResponse->ResponseTeardown();
}

void RtspSession::ResponseGetParameter()
{

}

BOOL RtspSession::TransportDescribe(string* describe)
{
	string sessionName;
	m_pRtspResponse->GetRequestSessionName(&sessionName);

	m_SessionName = sessionName;

	string localIpAddr = inet_ntoa(m_pRtspResponse->GetBindAddr().sin_addr);
	string targetIpAddr = inet_ntoa(m_pRtspResponse->GetConnectAddr().sin_addr);

	string sdp = RtspGenerateMediaSdp(localIpAddr.c_str(), targetIpAddr.c_str(), TRUE);

	if ( !sdp.length() )
		return FALSE;

	m_SetupUrl = "";

	if (describe)
		*describe = sdp;

	return TRUE;
}

BOOL RtspSession::TransportSetup(	string* pBindIp, INT* pBindPort, 
									string* pTargetIp, INT* pTargetPort, 
									INT32* pSsrc)
{
	string			sessionName;
	string			streamName;
	MediaStream*	pMediaStream;
	INT32			ssrc;

	ssrc = (INT32)GenerateOneNumber();
	if (pSsrc)
		*pSsrc = ssrc;

	// getsessionname ///trj
	if ( !m_pRtspResponse->GetRequestSessionName(&sessionName) )
		sessionName = m_SessionName;

	// 获取stream
	if ( !m_pRtspResponse->GetRequestStreamName(&streamName) )
	{
		//m_pRtspResponse->ResponseError("RTSP/1.0 400 Bad Request");
		//return FALSE;
	}

	//使用trackID设置stream
	string::size_type iFind = streamName.find("trackID=");
	if (iFind != string::npos)
	{
		streamName.erase(iFind, 8);
		int streamIdx = atoi(streamName.c_str()) - 1;

		pMediaStream = RtspGetMediaStream( streamIdx );
		if (!pMediaStream)
		{
			m_pRtspResponse->ResponseError("RTSP/1.0 404 Stream Not Found");
			return FALSE;
		}
		return StreamSetup(pMediaStream, pBindIp, pBindPort, pTargetIp, pTargetPort);
	}
	return FALSE;
}

BOOL RtspSession::StreamSetup(MediaStream* pMediaStream, string* pBindIp, INT* pBindPort, string* pTargetIp, INT* pTargetPort)
{
	SOCKADDR_IN addr;
	string		bindIp;
	INT			bindPort;
	string		targetIp;
	INT			targetPort;

	if (pMediaStream == NULL)
		return FALSE;

	addr		= m_pRtspResponse->GetBindAddr();
	bindIp		= inet_ntoa(addr.sin_addr);

	addr		= m_pRtspResponse->GetConnectAddr();
	targetIp	= inet_ntoa(addr.sin_addr);

	if ( !m_pRtspResponse->GetRequestTransportInfo(&targetPort, NULL) )
	{
		m_pRtspResponse->ResponseError("RTSP/1.0 400 Bad Request");
		return FALSE;
	}

	// 从8000端口开始 尝试绑定偶数端口

	bindPort = 8000;
	while( !pMediaStream->TransportSetup(bindIp.c_str(), bindPort, targetIp.c_str(), targetPort) )
	{
		bindPort += 2;

		if (bindPort >= 65530)
			return FALSE;
	}

	// 保存SETUP的url地址 用于PLAY时生成RTP-info
	string mrl;
	m_pRtspResponse->GetRequestMrl(&mrl);

	if (m_SetupUrl.length())
		m_SetupUrl += ',';
	m_SetupUrl += ("url=" + mrl);

	if (pBindIp)
		*pBindIp = bindIp;
	
	if (pBindPort)
		*pBindPort = bindPort;

	if (pTargetIp)
		*pTargetIp = targetIp;
	
	if (pTargetPort)
		*pTargetPort = targetPort;

	return TRUE;
}

BOOL RtspSession::TransportPlay()
{
	string			sessionName;

	if ( !m_pRtspResponse->GetRequestSessionName(&sessionName) )
	{
		m_pRtspResponse->ResponseError("RTSP/1.0 400 Bad Request");
		return FALSE;
	}
	
	UINT streamCount = GetMediaStreamCount();

	for (UINT iStream = 0; iStream < streamCount; iStream++)
	{
		MediaStream* pStream = RtspGetMediaStream(iStream);
		if (!pStream)
			continue;
		pStream->TransportStart();
	}

	return TRUE;
}

BOOL RtspSession::TransportPause()
{
		string			sessionName;
	MediaSession*	pMediaSession;

	if ( !m_pRtspResponse->GetRequestSessionName(&sessionName) )
	{
		m_pRtspResponse->ResponseError("RTSP/1.0 400 Bad Request");
		return FALSE;
	}
	
	UINT streamCount = GetMediaStreamCount();

	for (UINT iStream = 0; iStream < streamCount; iStream++)
	{
		MediaStream* pStream = RtspGetMediaStream(iStream);
		if (!pStream)
			continue;
		pStream->TransportStop();
	}

	return TRUE;
}

BOOL RtspSession::TransportTeardown()
{
		string			sessionName;
	MediaSession*	pMediaSession;

	if ( !m_pRtspResponse->GetRequestSessionName(&sessionName) )
	{
		m_pRtspResponse->ResponseError("RTSP/1.0 400 Bad Request");
		return FALSE;
	}
	
	UINT streamCount = GetMediaStreamCount();

	for (UINT iStream = 0; iStream < streamCount; iStream++)
	{
		MediaStream* pStream = RtspGetMediaStream(iStream);
		if (!pStream)
			continue;
		pStream->TransportTeardown();
	}
	
	// 清空保存SETUP的url地址 在SETUP时再重新设置
	m_SetupUrl = "";

	return TRUE;
}
#define LONGLONG long long
LONGLONG RtspSession::GenerateOneNumber() 
{
	///////////////////trj///////////////////////////////
	struct timeval tv;
	ulong time_val;
	gettimeofday(&tv, NULL);
	time_val = tv.tv_sec*1000L + tv.tv_usec/1000L;
	return time_val;
}


RtspSession::RtspSession(SOCKET s, SOCKADDR_IN bindAddr, SOCKADDR_IN connectAddr)
{
	Rtp_Port_Off = 0;/////trj 2014.10.16///no need when client have diff session name

	m_pRtspResponse = new RtspResponse(s, bindAddr, connectAddr);

	MediaStream* TmpMediaStream = new MediaStreamH264("h264");

	((MediaStreamH264*)TmpMediaStream)->Init(0,NETWORK_MTU,SpsWidth,SpsHeight,SpsProfileIdc,Sps_Pps_Param_Sets);//////////////trj 2014.10.20

	m_MediaStreams.push_back(TmpMediaStream);

	begin_thread(StartRtspSessionThread,this);
}

MediaStream* RtspSession::RtspGetMediaStream(UINT streamIdx)
{
		if (streamIdx >= m_MediaStreams.size() )
		return NULL;

	return m_MediaStreams[streamIdx];
}

UINT RtspSession::GetMediaStreamCount()
{
	return m_MediaStreams.size();
}

string	RtspSession::RtspGenerateMediaSdp(PCSTR localIpAddr, PCSTR targetIpAddr, BOOL bUseRTSP)
{
	string	userName, sessionId, sessionVersion;
	string	startTime, stopTime;
	string	sessionName;

	char	numberStr[100];
	snprintf(numberStr, 100, "%I64d", GenerateOneNumber());

	string	mediaSdp;
	int		rtpPayloadType = 96;

	sessionId		= numberStr;
	sessionVersion	= sessionId;

	sessionName		= m_SessionName;//m_Name;
	
	startTime		= "0";
	stopTime		= "0";

	mediaSdp += "v=0\r\n";		// v=0 sdp版本
	mediaSdp += "o=- "+sessionId+" "+sessionVersion+" "+"IN IP4 "+localIpAddr+"\r\n"; 	//- 无用户 IN Internet IP4 ip地址类型
	mediaSdp += "s="+sessionName+"\r\n";
	//mediaSdp += "b=AS:50\r\n";
	mediaSdp += "c=IN IP4 "+string(targetIpAddr)+"\r\n";
	mediaSdp += "t="+startTime+" "+stopTime+"\r\n";
	//mediaSdp += "a=x-broadcastcontrol:RTSP\r\n";
	if (bUseRTSP)
		mediaSdp += "a=control:*\r\n";

#if 0 //test
	mediaSdp += "a=isma-compliance:2,2.0,2\r\n";
#endif 

	for (UINT iStream = 0; iStream < m_MediaStreams.size(); iStream++)
	{
		mediaSdp += m_MediaStreams[iStream]->GenerateMediaSdp(rtpPayloadType++, bUseRTSP);
	}	
	return mediaSdp;
}
