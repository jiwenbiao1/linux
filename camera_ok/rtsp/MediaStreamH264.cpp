#include "StdAfx.h"
#include "MediaStreamH264.h"
#include "BaseEncoder.h"
MediaStreamH264::MediaStreamH264(LPCSTR mediaStreamName)
		:MediaStream(mediaStreamName, MEDIA_STREAM_TYPE_H264)
{
	m_nWidth = 0;
	m_nHeight = 0;
}

MediaStreamH264::~MediaStreamH264()
{

}

BOOL MediaStreamH264::Init(UINT nStreamBitrate, UINT nMTU, UINT nWidth, UINT nHeight, int profileLevelId, const string& spropParameterSets)
{
	m_nWidth = nWidth;
	m_nHeight = nHeight;
	m_ProfileLevelId = profileLevelId;
	m_SpropParameterSets = spropParameterSets;
	return MediaStream::Init(nStreamBitrate, nMTU);
}

string MediaStreamH264::GenerateMediaSdp(UINT nRtpPayloadType, BOOL bUseRTSP)
{
	string	mediaSdp;
	
	string	port;
	string	payloadType;
	string	bs;
	string	fmtp;
	string	streamid;
	string	esid;
	string	cliprect;

	char	temp[500];
	
	m_nRtpPayloadType = nRtpPayloadType;


	// 生成各字段的内容
	snprintf(temp, 500, "%u", m_nRtpPort);
	port			= temp;

	snprintf(temp, 500, "%u", m_nRtpPayloadType);
	payloadType		= temp;

	snprintf(temp, 500, "profile-level-id=%06X; sprop-parameter-sets=%s; packetization-mode=1;", 
																m_ProfileLevelId, m_SpropParameterSets.c_str());
	fmtp			= temp;

	snprintf(temp, 500, "0,0,%u,%u", m_nHeight, m_nWidth);
	cliprect = temp;

	snprintf(temp, 500, "%u", m_nBandWidth);
	bs = temp;
	
	snprintf(temp, 500, "%u", m_nRtpPayloadType - 96 + 1);
	streamid = temp;

	snprintf(temp, 500, "%u", m_nRtpPayloadType - 96 + 201);
	esid = temp;

	// 生成sdp内容
	mediaSdp += "m=video "+port+" RTP/AVP "+payloadType+"\r\n";	//m
	mediaSdp += "b=AS:"+bs+"\r\n";													//b																						
	mediaSdp += "a=rtpmap:"+payloadType+" H264/90000\r\n";							//a=rtpmap
	mediaSdp += "a=fmtp:"+payloadType+" "+fmtp+"\r\n";								//a=fmtp
	mediaSdp += "a=cliprect:"+cliprect+"\r\n";										//a=cliprect
	mediaSdp += "a=mpeg4-esid:"+esid+"\r\n";
	if (bUseRTSP)
		mediaSdp += "a=control:trackID="+streamid+"\r\n";
	
	return mediaSdp;
}

UINT MediaStreamH264::TransportData(PBYTE pData, UINT dataSize, int pts)
{
	//////Maybe One Than More Nalu in One Frame/////////////trj note:///
	int Ret = 0;

	Ret = TransportH264Nal(pData, dataSize, pts, 1);

	return Ret;
}

UINT MediaStreamH264::TransportH264Nal(PBYTE pNal, UINT nalSize, INT32 pts, BOOL isLast)
{
	if (m_bRun == FALSE)
		return 0;

	if( nalSize < 5 )
		return 0;

	UINT mtu = m_nMTU;

	const int i_max = mtu - RTP_HEADER_SIZE;
	int i_nal_hdr;
	int i_nal_type;

	i_nal_hdr = pNal[3];
	i_nal_type = i_nal_hdr&0x1f;

	if( i_nal_type == 7 || i_nal_type == 8 )
	{
		return 0;
	}

	PBYTE p_data = pNal;
	int	i_data = nalSize;

	p_data += 3;
	i_data -= 3;

	int writeSize = 0;

	if( i_data <= i_max )
	{
		writeSize = m_pRtpTransport->Write(p_data, i_data, m_nRtpPayloadType, pts, 0, isLast);
		if (writeSize <= 0)
			return 0;
		return writeSize;
	}
	else
	{
		const int i_count = ( i_data - 1 + i_max - 2 - 1 ) / ( i_max - 2);////use a little math skill///trj note:
		int i;

		///trj:note//because of Fragment////Skip Nalu Header Byte
		p_data++;
		i_data--;

		BYTE FU_Payload_Data[2000];

		for( i = 0; i < i_count; i++ )
		{
			const int i_payload =  (i_data < (i_max - 2)) ? i_data : (i_max - 2);
			const int nalSize = 2 + i_payload;

			//FU indicator
			FU_Payload_Data[0] = 0x00 | ( i_nal_hdr & 0x60) | 28;
			
			//FU header
			FU_Payload_Data[1] = ( i == 0 ? 0x80 : 0x00 ) | ( (i == i_count-1) ? 0x40 : 0x00 )  | i_nal_type;

			//FU payload
			memcpy( &FU_Payload_Data[2], p_data, i_payload );

			int iWrite = m_pRtpTransport->Write(FU_Payload_Data, nalSize, m_nRtpPayloadType, pts, 0, isLast && (i == i_count - 1));
			if (iWrite > 0)
				writeSize += iWrite;

			i_data -= i_payload;

			p_data += i_payload;
		}
	}
	return writeSize;
}
