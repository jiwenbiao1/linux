#include "StdAfx.h"
#include "RtspService_t.h"

#include "RtspService.h"
#include<pthread.h>

RtspService_t::RtspService_t()
{
	m_pRtspTransport = NULL;
	
	m_pMediaSession = NULL;
	m_pMediaStream_Video = NULL;
	m_pMediaStream_Audio = NULL;
	
	m_hVideoFile = NULL;
	m_hAudioFile = NULL;

	m_hVideoFile_ReadThread = NULL;
	m_bVideoFile_ReadThread_Stop = FALSE;

	m_hAudioFile_ReadThread = NULL;
	m_bAudioFile_ReadThread_Stop = FALSE;

	m_bOpen = FALSE;

	pFile = NULL;
}

RtspService_t::~RtspService_t()
{
	Release();
}

void RtspService_t::Release()
{
	m_hAudioFile = NULL;

	m_bOpen = FALSE;
}

BOOL RtspService_t::Open(LPCTSTR textVideoFilePath, LPCTSTR textAudioFilePath)
{
	if (!textVideoFilePath )
		return FALSE;

	if (m_bOpen)
		return TRUE;

	pFile = fopen("test.264","rb");
	if(pFile == NULL)
	{
		printf("open video source file failed\n");
	}

	begin_thread(H264File_Read2SendThread,this);

	m_bOpen = TRUE;

	return TRUE;
}
