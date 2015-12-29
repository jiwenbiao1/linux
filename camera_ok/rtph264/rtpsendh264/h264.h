#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#pragma pack (1)

#define PACKET_BUFFER_END	(unsigned int)0x00000000
#define MAX_RTP_PKT_LENGTH	1400
#define DEST_IP				"192.168.169.100"
#define DEST_PORT			1234
#define H264				96

#define TRUE				1
#define FALSE				0
//typedef char				bool;

typedef struct
{
	unsigned char csrc_len:4;
	unsigned char extension:1;
	unsigned char padding:1;
	unsigned char version:2;
	unsigned char payload:7;//��������96 h264
	unsigned char marker:1;
	unsigned short seq_no;//16bit ���к� Sequence number(SN)
	unsigned long timestamp;//32bitʱ���
	unsigned long ssrc;
} RTP_FIXED_HEADER;

//1�ֽ�
typedef struct {
	unsigned char TYPE:5;//nal_unit_type. ���NALU��Ԫ������.
	unsigned char NRI:2;//nal_ref_idc. ȡ00~11,�ƺ�ָʾ���NALU����Ҫ��,��00��NALU���������Զ���������Ӱ��ͼ��Ļط�
	unsigned char F:1;//forbidden_zero_bit �� H.264 �淶�й涨����һλ����Ϊ 0.
} NALU_HEADER;

typedef struct {
	unsigned char TYPE:5; //28 ��Ƭ��Ԫ��
	unsigned char NRI:2;
	unsigned char F:1;
} FU_INDICATOR;

//    S bitΪ1��ʾ��Ƭ��NAL��ʼ������Ϊ1ʱ��E����Ϊ1
//   E bitΪ1��ʾ����������Ϊ1��S����Ϊ1
//   R bit����λ
//  Type����NALUͷ�е�Type,ȡ1-23���Ǹ�ֵ
typedef struct {
	unsigned char TYPE:5;
	unsigned char R:1;
	unsigned char E:1;
	unsigned char S:1;
} FU_HEADER;;

