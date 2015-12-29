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
	unsigned char payload:7;//负载类型96 h264
	unsigned char marker:1;
	unsigned short seq_no;//16bit 序列号 Sequence number(SN)
	unsigned long timestamp;//32bit时间戳
	unsigned long ssrc;
} RTP_FIXED_HEADER;

//1字节
typedef struct {
	unsigned char TYPE:5;//nal_unit_type. 这个NALU单元的类型.
	unsigned char NRI:2;//nal_ref_idc. 取00~11,似乎指示这个NALU的重要性,如00的NALU解码器可以丢弃它而不影响图像的回放
	unsigned char F:1;//forbidden_zero_bit 在 H.264 规范中规定了这一位必须为 0.
} NALU_HEADER;

typedef struct {
	unsigned char TYPE:5; //28 分片单元包
	unsigned char NRI:2;
	unsigned char F:1;
} FU_INDICATOR;

//    S bit为1表示分片的NAL开始，当它为1时，E不能为1
//   E bit为1表示结束，当它为1，S不能为1
//   R bit保留位
//  Type就是NALU头中的Type,取1-23的那个值
typedef struct {
	unsigned char TYPE:5;
	unsigned char R:1;
	unsigned char E:1;
	unsigned char S:1;
} FU_HEADER;;

