#include "stdafx.h"
#include "RtspService.h"
#include "RtspService_t.h"

#include "MediaStreamH264.h"

#define TCHAR char
RtspService_t* g_pRtspService;

int begin_thread(thread_func func,void* arg)
{
	int ret = 0;
	pthread_t pid;
	pthread_attr_t attr;

	pthread_attr_init(&attr);

	if (pthread_create(&pid,&attr,func,arg)) 
	{
		ret = errno;
		printf("pthread_create(func) failed with %d %d\n", ret, ENOMEM);
	}
	return ret;
}

int filePos = 0;///trj
struct Nalu nalu[2000];
int NaluCount = 0;

unsigned char memFile[3000000];
int fileLen = 0;


int FindStartCode(size_t pos)
{
	int ret = 0;
	while(pos < fileLen)
	{
		if(memFile[pos] == 0 && memFile[pos + 1] == 0 && memFile[pos + 2] == 0 && memFile[pos + 3] == 1)
		{
//			NaluCount++;

			break;
		}
		pos++;
	}

	ret = pos;

	return ret;
}


int Memory_2_Nalu()
{
	int ret = 0;

	size_t StartCodePosition = 0;
	size_t NextStartCodePosition = 0;
	size_t frameLen = 0;
	unsigned char* pTmp = memFile;
	StartCodePosition = FindStartCode(0);
	while(1)
	{
		NextStartCodePosition = FindStartCode(StartCodePosition + 5);

		NaluCount++;

		frameLen = NextStartCodePosition - StartCodePosition;

		if(NaluCount < 10)
		{
//			printf("trj test frameLen = %d\n",frameLen);
		}

		memcpy(nalu[NaluCount - 1].data,(char*)pTmp + StartCodePosition,frameLen);

		nalu[NaluCount - 1 ].len = frameLen;

//		ProcessNaluUnit(StartCodePosition,frameLen);

		if(NextStartCodePosition == fileLen)
		{
//			printf("H264 file parse over\n");

			break;
		}

		StartCodePosition = NextStartCodePosition;
	}

//	printf("the H264 file have %d Nal unit\n",NaluCount);

	return ret;
}

UINT SpsWidth = 0;
UINT SpsHeight = 0;
int SpsProfileIdc;
string Sps_Pps_Param_Sets;

void* H264File_Read2SendThread(void* pUser)
{
	RtspService_t* pRtspService = (RtspService_t*)pUser;

	string		sps_base64;
	string		pps_base64;

	bool_t		b_result;

	bool_t		b_find_sps = FALSE;
	bool_t		b_find_pps = FALSE;
	bool_t		found_frame_start = FALSE;
	uint32_t	i_begin_time = 0;
	uint32_t	i_current_time = 0;
	int64_t		frame_count = 0;
	int64_t		iframe_offset = 0;

	h264_sps_t	sps;

	int i_nal_type;

	int SendNaluCount = 0;
	
	memset(&sps, 0, sizeof(h264_sps_t));
///////////////////////read file_2_nalu ///////////////trj note://////

	{
		FILE* pf = fopen("test.264","rb");

		if(pf == NULL)
		{
			printf("open file failed \n");
		}

		size_t readSize = fread(memFile,1,3000000,pf);

		fileLen = readSize;

		Memory_2_Nalu();

		fclose(pf);

		pf = NULL;
	}

	while(SendNaluCount < NaluCount)
	{
		h264_get_nal_type(&i_nal_type, nalu[SendNaluCount].data + 1);

		if ( !b_find_sps )
		{
			if ( i_nal_type == 7)
			{
				uint8_t tmpBuffer[1024];

				int tmpLen = 0;

				h264_decode_annexb(tmpBuffer,(INT *)&tmpLen,nalu[SendNaluCount].data + 5, nalu[SendNaluCount].len - 5);
				
				h264_decode_seq_parameter_set(tmpBuffer,tmpLen,&sps);
				if (sps.time_scale)
				{
					BaseEncoder::Base64Encode(nalu[SendNaluCount].data + 1, nalu[SendNaluCount].len - 1, &sps_base64);

					b_find_sps = TRUE;
				}
				else
				{
					printf("The SPS's time_scale field info is error!\n");
				}
			}
		}

		if ( !b_find_pps )
		{
			if ( i_nal_type == 8 )
			{
				BaseEncoder::Base64Encode(nalu[SendNaluCount].data + 1, nalu[SendNaluCount].len - 1, &pps_base64);
				b_find_pps = TRUE;
			}

			if (b_find_sps && b_find_pps)
			{
				string sprop_parameter_sets;
				sprop_parameter_sets = sps_base64;
				sprop_parameter_sets += ',';
				sprop_parameter_sets += pps_base64;

				SpsWidth = sps.mb_width;
				SpsHeight = sps.mb_height;
				SpsProfileIdc = sps.profile_idc;
				Sps_Pps_Param_Sets = sprop_parameter_sets;
			}
		}

		SendNaluCount++;

		if(b_find_pps && b_find_sps)
		{
			break;
		}
	}

	SendNaluCount = 0;

	while(SendNaluCount < NaluCount)
	{
		usleep(60 * 1000);

		if(g_pRtspService->m_pRtspTransport->m_RtspSessionList.size() == 0)
			continue;
		
		h264_get_nal_type(&i_nal_type, nalu[SendNaluCount].data + 1);
		
/////////trj note: temp one nalu is a frame////////////////////
//		if ( h264_find_frame_end(&found_frame_start, nalu[SendNaluCount].data + 1, nalu[SendNaluCount].len - 1, i_nal_type) )
		{
			frame_count++;
			
			// 播出数据，注意这里的时钟频率是按90k计算
			int64_t i_pts = h264_get_pts_rtp(&iframe_offset, frame_count, &sps, nalu[SendNaluCount].data + 1, nalu[SendNaluCount].len - 1, i_nal_type);

			for(int iPlayCount = 0 ; iPlayCount < pRtspService->m_pRtspTransport->m_RtspSessionList.size();iPlayCount++)
			{
				if(pRtspService->m_pRtspTransport->m_RtspSessionList[iPlayCount]->m_MediaStreams[0]->m_bRun == FALSE)
				{
					continue;
				}
				pRtspService->m_pRtspTransport->m_RtspSessionList[iPlayCount]->m_MediaStreams[0]->TransportData(nalu[SendNaluCount].data + 1, nalu[SendNaluCount].len - 1, i_pts);
			}
		}
		SendNaluCount++;

		SendNaluCount = SendNaluCount % NaluCount;
	}
	return NULL;
}

int main(int argc, TCHAR* argv[], TCHAR* envp[])
{	
	string	textBindIP;
	UINT	nBindPort;
	string	textVideoFile;

	// 创建Rtsp Transport对象
	MediaSession* TmpMediaoSession = NULL;

	MediaStream* TmpMediaStream = NULL;

	g_pRtspService = new RtspService_t();

	g_pRtspService->m_pRtspTransport = new RtspTransport();

	begin_thread(H264File_Read2SendThread,(void*)g_pRtspService);


	// 打开Rtsp服务
	if ( !g_pRtspService->m_pRtspTransport->Open("192.168.169.100", 1234, 0xFFFFFFFF) )
	{
		printf("Rtsp Transport open failed.\n");
		delete g_pRtspService->m_pRtspTransport;
		delete g_pRtspService;
	}

	while(1)
	{
		usleep(20 * 1000);
	}

	delete g_pRtspService->m_pRtspTransport;
	delete g_pRtspService;

	return 0;
}