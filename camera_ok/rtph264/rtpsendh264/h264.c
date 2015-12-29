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

FILE *bits = NULL;                //< the bit stream file
static int findStartCode2 (unsigned char *Buf); // find start code 0x000001
static int findStartCode3 (unsigned char *Buf); // find start code 0x00000001
//static bool flag = true;
static int info2 = 0, info3 = 0;

RTP_FIXED_HEADER	*rtp_hdr;
NALU_HEADER		*nalu_hdr;
FU_INDICATOR		*fu_ind;
FU_HEADER		*fu_hdr;

void openBitstreamFile (char *fn)
{
	if (NULL == (bits=fopen(fn, "rb")))
	{
		printf("open file error\n");
		exit(0);
	}
}

//malloc memory for struct NALU_t
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

////这个函数输入为一个NAL结构体，主要功能为得到一个完整的NALU并保存在NALU_t的buf中，获取他的长度，填充F,IDC,TYPE位。
//并且返回两个开始字符之间间隔的字节数，即包含有前缀的NALU的长度
int getAnnexbNALU (NALU_t *nalu)
{
	int pos = 0;
	int startCodeFound, rewind;
	unsigned char *Buf;

	if ((Buf = (unsigned char*)calloc (nalu->max_size , sizeof(char))) == NULL)
		printf ("GetAnnexbNALU: Could not allocate Buf memory\n");

	nalu->startcodeprefix_len = 3; // init start code prefix len as 3
	if (3 != fread(Buf, 1, 3, bits)) // read 3 byte from stream
	{
		free(Buf);
		return 0;
	}

	info2 = findStartCode2 (Buf); // check is 0x000001 or not
	// is not 0x000001
	if(info2 != 1)
	{
		//read more one byte
		if(1 != fread(Buf+3, 1, 1, bits))
		{
			free(Buf);
			return 0;
		}

		info3 = findStartCode3 (Buf); // check is 0x00000001 or not
		// is not 0x00000001
		if (info3 != 1)
		{
			free(Buf);
			return -1;
		}
		else
		{
			pos = 4;
			nalu->startcodeprefix_len = 4; // init start code prefix len as 4
		}
	}else{
		nalu->startcodeprefix_len = 3; // init start code prefix len as 3
		pos = 3;
	}
	// find next start code
	startCodeFound = 0;
	info2 = 0;
	info3 = 0;

	while (!startCodeFound)
	{
		if (feof (bits))// read arrive at file tail
		{
			nalu->len = (pos-1)-nalu->startcodeprefix_len;
			memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);
			nalu->forbidden_bit = nalu->buf[0] & 0x80; // 1 bit   
			nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
			nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
			free(Buf);
			return pos-1;
		}
		Buf[pos++] = fgetc (bits);// read more byte into Buf
		info3 = findStartCode3(&Buf[pos-4]); // check is 0x00000001 or not
		if(info3 != 1)
			info2 = findStartCode2(&Buf[pos-3]); // check is 0x000001 or not
		startCodeFound = (info2 == 1 || info3 == 1);
	}


	// Here, we have found another start code (and read length of startcode bytes more than we should
	// have.  Hence, go back in the file
	rewind = (info3 == 1)? -4 : -3;
//改变流指针
	if (0 != fseek (bits, rewind, SEEK_CUR))// move file pointer to prev NALU tail
	{
		free(Buf);
		printf("GetAnnexbNALU: Cannot fseek in the bit stream file");
	}

	// Here the Start code, the complete NALU, and the next start code is in the Buf.
	// The size of Buf is pos, pos+rewind are the number of bytes excluding the next start code
	// (pos+rewind)-startcodeprefix_len is the size of the NALU excluding the start code

	nalu->len = (pos+rewind)-nalu->startcodeprefix_len;
	memcpy (nalu->buf, &Buf[nalu->startcodeprefix_len], nalu->len);//copy a NALU
	nalu->forbidden_bit = nalu->buf[0] & 0x80; // 1 bit
	nalu->nal_reference_idc = nalu->buf[0] & 0x60; // 2 bit
	nalu->nal_unit_type = (nalu->buf[0]) & 0x1f;// 5 bit
	free(Buf);

	return (pos+rewind);// return length between two start code (contain prev start code )
}

// print the NALU length and type
void dump(NALU_t *n)
{
	if (!n)return;
	//printf("a new nal:");
	printf(" len: %d  ", n->len);
	printf("nal_unit_type: %x\n", n->nal_unit_type);
}

int main(int argc, char **argv){
	openBitstreamFile("../video.h264");//open h264 file
	NALU_t*				n;
	char*				nalu_payload;
	char					sendbuf[1500];

	unsigned short			seq_num =0;
	int					bytes=0;
	int					socket1;
	
	struct sockaddr_in		server;
	int					len =sizeof(server);
	float					framerate=29;
	unsigned int			timestamp_increse=0;
	unsigned int			ts_current=0;
	
	timestamp_increse		= (unsigned int)(90000.0 / framerate);
	server.sin_family		= AF_INET;
	server.sin_port			= htons(DEST_PORT);   
	server.sin_addr.s_addr	= inet_addr(DEST_IP);
	socket1 = socket(AF_INET, SOCK_DGRAM, 0);//udp 广播 局域网
	connect(socket1, (struct sockaddr *)&server, len) ;
	n = allocNALU(8000000);//1frame??

	while(!feof(bits)) //读到文件结束符eof
	{
		/* after call getAnnexbNALU(),file pointer will point to prev NALU tail 
		  * meanwhile the position is the next NALU start code 0x000001 header
		  *///找到nalu结尾的位置 改变流指针
		if(getAnnexbNALU(n) < 1){
			printf("unknow format,must start with 0x000001 or 0x00000001\r\n");
			exit(0);
		}
		 dump(n);
		//if (n->nal_unit_type == 5) dump(n); //print the NALU length and type

		// clear sendbuf
		memset(sendbuf, 0 ,1500);
		//sendbuf 1500     0~12B rtp_header + NALU(<=1400)
		// set sendbuf[0] as rtp_hdr
		rtp_hdr =(RTP_FIXED_HEADER*)&sendbuf[0]; 
		// set RTP HEADER
		// rtp_hdr->csrc_len	= 0;
		// rtp_hdr->extension	= 0;
		// rtp_hdr->padding		= 0;
		rtp_hdr->version		= 2;  // RTP Version
		rtp_hdr->payload		= H264;  // payload type
		rtp_hdr->marker		= 0;   // defined by protocol
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
	}

	freeNALU(n);
	close(socket1);
	fclose(bits);
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