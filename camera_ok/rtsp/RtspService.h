#pragma once

#include "RtspService_t.h"
#include "H264.h"
#include "BaseEncoder.h"
#include "MediaStreamH264.h"

#define NETWORK_MTU 1460
#define FILE_READ_SIZE 1024*1024*2

#define  PACE_LENGTH 200 * 1024/////trj added 200k

extern int filePos;

struct Nalu
{
	unsigned char data[1024 * 20];

	int len;
};
extern struct Nalu nalu[2000];
extern int NaluCount;

extern unsigned char memFile[3000000];
extern int fileLen;

extern int FindStartCode(size_t pos);
extern int Memory_2_Nalu();

extern void* H264File_Read2SendThread(void* pUser);

extern RtspService_t* g_pRtspService;

extern UINT SpsWidth;
extern UINT SpsHeight;
extern int SpsProfileIdc;
extern string Sps_Pps_Param_Sets;

