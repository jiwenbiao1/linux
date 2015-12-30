#include<string.h>
#include "stdafx.h"
#include "Rtp.h"

Rtp::Rtp(UINT mtu) :Udp(mtu)
{
	m_SequenceNumber = 0;
}

Rtp::~Rtp()
{
}

int Rtp::Read(BYTE* pBuffer, UINT16 bufferSize, INT8* pPayloadType, UINT16* pSequenceNumber, INT32* pTimeStamp, INT32* pSsrc, UINT nTimeOut)
{
	return 0;
}

int Rtp::Write(PBYTE pBuffer, UINT16 bufferSize, INT8 payloadType, INT32 timeStamp, INT32 ssrc, BOOL marker, UINT nTimeOut)
{
	int iWrite;

	CreateRtpPacket(pBuffer, bufferSize, payloadType, timeStamp, ssrc, marker);

	iWrite = Udp::Write(Rtp_Packet_Data, Rtp_Packet_Len, nTimeOut);
	if (iWrite < RTP_HEADER_SIZE)
		return -1;

	return iWrite - RTP_HEADER_SIZE;
}

void Rtp::CreateRtpPacket(PBYTE pData, UINT16 dataSize, UINT8 nPayloadType, INT32 timeStamp, INT32 nSSRC, BOOL marker)
{
	int Ret = 0;

	Rtp_Packet_Data[0] = 0x80;
	Rtp_Packet_Data[1] = (marker?0x80:0x00)|nPayloadType;

	Rtp_Packet_Data[2] = ( m_SequenceNumber >> 8 )&0xff;
	Rtp_Packet_Data[3] = m_SequenceNumber&0xff;

	Rtp_Packet_Data[4] = (BYTE)( timeStamp >> 24 )&0xff;
	Rtp_Packet_Data[5] = (BYTE)( timeStamp >> 16 )&0xff;
	Rtp_Packet_Data[6] = (BYTE)( timeStamp >>  8 )&0xff;
	Rtp_Packet_Data[7] = (BYTE)timeStamp&0xff;

	Rtp_Packet_Data[ 8] = ( nSSRC >> 24 )&0xff;
	Rtp_Packet_Data[ 9] = ( nSSRC >> 16 )&0xff;
	Rtp_Packet_Data[10] = ( nSSRC >>  8 )&0xff;
	Rtp_Packet_Data[11] = nSSRC&0xff;

	m_SequenceNumber++;

	memcpy(&Rtp_Packet_Data[12],pData,dataSize);

	Rtp_Packet_Len = dataSize + RTP_HEADER_SIZE;

	return;
}

BOOL Rtp::ParseRtpHeader(PBYTE pRtpHeader, UINT8* pPayloadType, UINT16* pSequenceNumber, INT32* pTimeStamp, INT32* pSsrc)
{
	if (pRtpHeader[0] != 0x80)
		return FALSE;

	*pPayloadType	= pRtpHeader[1]; 
	*pSequenceNumber = ((WORD)pRtpHeader[2]) << 8 | pRtpHeader[3];
	*pTimeStamp = (INT32)pRtpHeader[4] << 24 | (INT32)pRtpHeader[5] << 16 | (INT32)pRtpHeader[6] << 8 | pRtpHeader[7];
	*pSsrc = (INT32)pRtpHeader[8] << 24 | (INT32)pRtpHeader[9] << 16 | (INT32)pRtpHeader[10] << 8 | pRtpHeader[11];

	return TRUE;
}