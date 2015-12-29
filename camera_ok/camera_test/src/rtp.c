#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "h264.h"
typedef struct
{
	int startcodeprefix_len;	// 4 for parameter sets and first slice in picture, 3 for everything else (suggested)
	unsigned len;			// Length of the NAL unit (Excluding the start code, which does not belong to the NALU)
	unsigned max_size;		// Nal Unit Buffer size
	int forbidden_bit;		// should be always FALSE
	int nal_reference_idc;	// NALU_PRIORITY_xxxx
	int nal_unit_type;		// NALU_TYPE_xxxx    
	char *buf;			// contains the first byte followed by the EBSP
	unsigned short lost_packets;  // true, if packet loss is detected
} NALU_t;

NALU_t *allocNALU(int buffersize)
{
	NALU_t *n;

	if ((n = (NALU_t*)calloc (1, sizeof (NALU_t))) == NULL)
	{
		printf("AllocNALU: n");
		exit(0);
	}

	n->max_size=buffersize;

	if ((n->buf = (char*)calloc (buffersize, sizeof (char))) == NULL)
	{
		free (n);
		printf ("AllocNALU: n->buf");
		exit(0);
	}

	return n;
}

void freeNALU(NALU_t *n)
{
	if (n)
	{
		if (n->buf)
		{
			free(n->buf);
			n->buf=NULL;
		}
		free (n);
	}
}


void dump(NALU_t *n)
{
	if (!n)return;
	//printf("a new nal:");
	printf(" len: %d  ", n->len);
	printf("nal_unit_type: %x\n", n->nal_unit_type);
}




//全局
		unsigned short		seq_num =0;
		int 				socket1;
		struct sockaddr_in	server;
		int 				bytes=0;
		float					framerate=10;
		unsigned int			timestamp_increse = 0;
		unsigned int			ts_current=0;
				
void initSocket()
{		timestamp_increse = (unsigned int)(90000.0 / framerate);
		int 				len =sizeof(server);
		server.sin_family		= AF_INET;
		server.sin_port 		= htons(DEST_PORT);   
		server.sin_addr.s_addr	= inet_addr(DEST_IP);
		socket1 = socket(AF_INET, SOCK_DGRAM, 0);//udp 广播 局域网
		connect(socket1, (struct sockaddr *)&server, len) ;	
		
	
}



static int findStartCode2 (unsigned char *Buf)
{
	if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=1) return 0; // check buf is 0x000001, true return 1
	else return 1;
}

static int findStartCode3 (unsigned char *Buf)
{
	if(Buf[0]!=0 || Buf[1]!=0 || Buf[2] !=0 || Buf[3] !=1) return 0; // check buf is 0x00000001, true return 1
	else return 1;
}

void sendh264(uint8_t *h264_buf, size_t h264_length)
{
	
}

void sendh264frame(uint8_t *h264_buf, size_t h264_length)
{

		
		NALU_t *n;
		n = allocNALU(320*240*5);//1frame??8000000
		n->startcodeprefix_len = 4;
		//判断开始码
		if(findStartCode3(h264_buf)==1)
		{
			n->startcodeprefix_len = 4;
		}
		else 
		{
			n->startcodeprefix_len = 3;
		}
		n->len = h264_length - n->startcodeprefix_len;
		memcpy (n->buf, &h264_buf[n->startcodeprefix_len], n->len);//copy a NALU
		n->forbidden_bit = n->buf[0] & 0x80; // 1 bit
		n->nal_reference_idc = n->buf[0] & 0x60; // 2 bit
		n->nal_unit_type = (n->buf[0]) & 0x1f;// 5 bit
		
		RTP_FIXED_HEADER	*rtp_hdr;
		NALU_HEADER			*nalu_hdr;
		FU_INDICATOR		*fu_ind;
		FU_HEADER			*fu_hdr;
		char*				nalu_payload;

		char sendbuf[1500];
		memset(sendbuf, 0 ,1500);
		//printf("exec sendframe\n");
		//dump(n);
		initSocket();
		rtp_hdr =(RTP_FIXED_HEADER*)&sendbuf[0]; 
		rtp_hdr->version		= 2;  // RTP Version
		rtp_hdr->payload		= H264;  // payload type
		rtp_hdr->marker			= 0;   // defined by protocol
		rtp_hdr->ssrc			= htonl(12345678);    //define ssrc as random integer

		// NALU less than 1400 byte, send in one RTP packet 
		if(n->len<=1400)
		{
			// set RTP Header m
			rtp_hdr->marker=1;
			// set RTP Header sequence number
			rtp_hdr->seq_no     = htons(seq_num ++);
			
			//set NALU HEADER
			nalu_hdr = (NALU_HEADER*)&sendbuf[12];
			nalu_hdr->F = n->forbidden_bit;

			// Valid data on the 6,7 bit,must move 5bit to right
			nalu_hdr->NRI	= n->nal_reference_idc>>5;
			nalu_hdr->TYPE	= n->nal_unit_type;
			
			// get sendbuf[13] address assigned to nalu_payload
			nalu_payload=&sendbuf[13];
			// copy nalu to nalu_payload except start code
			memcpy(nalu_payload,n->buf+1,n->len-1);

			ts_current = ts_current + timestamp_increse;
			rtp_hdr->timestamp=htonl(ts_current);
			bytes=n->len + 12 ;
			// send rtp package
			send( socket1, sendbuf, bytes, 0 );
			//usleep(1000000);
		}

//nalu 切片
		else if(n->len>1400)
		{
			int k=0, l=0;
			k=n->len/1400; // need 'k' num 1400 pkg
			l=n->len%1400;// last RTP pkg load byte
			int t=0; // current send pkg index
			ts_current=ts_current+timestamp_increse; //use the same timestamp
			rtp_hdr->timestamp=htonl(ts_current);
			while(t<=k)
			{
				rtp_hdr->seq_no = htons(seq_num ++); // sequence ++
				if(!t)// first RTP pkg set set FU HEADER S:E:R = 1:0:0
				{
					// set RTP Header m
					rtp_hdr->marker=0;
					//set FU INDICATOR
					fu_ind =(FU_INDICATOR*)&sendbuf[12];
					fu_ind->F=n->forbidden_bit;
					fu_ind->NRI=n->nal_reference_idc>>5;
					fu_ind->TYPE=28;

					// set FU HEADER, get sendbuf[13] address assigned to fu_hdr
					fu_hdr =(FU_HEADER*)&sendbuf[13];
					fu_hdr->E=0;
					fu_hdr->R=0;
					fu_hdr->S=1;
					fu_hdr->TYPE=n->nal_unit_type;

					//  get sendbuf[14] address assigned to nalu_payload
					nalu_payload=&sendbuf[14];
					memcpy(nalu_payload,n->buf+1,1400);

					// rtp_header+fu_ind+fu_hdr = 14bit
					bytes=1400+14;
					send( socket1, sendbuf, bytes, 0 );
					t++;

				}
				
				else if(k==t)// last RTP pkg set set FU HEADER S:E:R = 0:1:0
				{

					// set RTP Header m
					// the last slice set m=1
					rtp_hdr->marker=1;
					// set FU INDICATOR
					fu_ind =(FU_INDICATOR*)&sendbuf[12];
					fu_ind->F=n->forbidden_bit;
					fu_ind->NRI=n->nal_reference_idc>>5;
					fu_ind->TYPE=28;

					// set FU HEADER
					fu_hdr =(FU_HEADER*)&sendbuf[13];
					fu_hdr->R=0;
					fu_hdr->S=0;
					fu_hdr->TYPE=n->nal_unit_type;
					fu_hdr->E=1;

					// set NALU payload
					nalu_payload=&sendbuf[14];
					memcpy(nalu_payload,n->buf+t*1400+1,l-1);
					bytes=l-1+14;
					send( socket1, sendbuf, bytes, 0 );
					t++;
				//	Sleep(100);
				}
				else if(t<k && 0 != t)
				{
					rtp_hdr->marker=0;
					fu_ind =(FU_INDICATOR*)&sendbuf[12];
					fu_ind->F=n->forbidden_bit;
					fu_ind->NRI=n->nal_reference_idc>>5;
					fu_ind->TYPE=28;

					fu_hdr =(FU_HEADER*)&sendbuf[13];
					//fu_hdr->E=0;
					fu_hdr->R=0;
					fu_hdr->S=0;
					fu_hdr->E=0;
					fu_hdr->TYPE=n->nal_unit_type;

					nalu_payload=&sendbuf[14];
					memcpy(nalu_payload,n->buf+t*1400+1,1400);
					bytes=1400+14;

					// set rtp package
					send( socket1, sendbuf, bytes, 0 );
					t++;
				}
			}
		}


		freeNALU(n);
}
	
