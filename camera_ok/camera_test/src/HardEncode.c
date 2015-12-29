#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>

#include "SsbSipH264Encode.h"
#include "SsbSipLogMsg.h"
//#include "performance.h"

static 	void   *handle;
int HardEncode_init(int width, int height)
{
	unsigned char	*p_inbuf;
	unsigned char	*p_outbuf;
	int				ret;
	int				slices[2];
	
    int frame_rate = 10;
	int bitrate = 1000;
	int gop_num = 1;
	

	handle = SsbSipH264EncodeInit(width, height, frame_rate, bitrate, gop_num);
	if (handle == NULL) {
		LOG_MSG(LOG_ERROR, "Test_Encoder", "SsbSipH264EncodeInit Failed\n");
		return -1;
	}
	// Setting multiple slices
	// This setting must be called before SsbSipH264EncodeExe()
	slices[0] = 0;	// 0 for single, 1 for multiple
	slices[1] = 4;	// count of slices
	SsbSipH264EncodeSetConfig(handle, H264_ENC_SETCONF_NUM_SLICES, slices);
	ret = SsbSipH264EncodeExe(handle);
	return 1;
}
void HardEncode_destory()
{
	
	SsbSipH264EncodeDeInit(handle);
}
int HardEncode_test(void* in_addr,size_t frame_length,void* out_addr,int width, int height, int frame_rate, int bitrate, int gop_num)
{
	unsigned char	*p_inbuf;
	unsigned char	*p_outbuf;
	int				ret;
	int				slices[2];
	long 			size;
    frame_rate = 10;
	bitrate = 1000;
	gop_num = 1;
	//static 	void   *handle;
/*
	handle = SsbSipH264EncodeInit(width, height, frame_rate, bitrate, gop_num);
	if (handle == NULL) {
		LOG_MSG(LOG_ERROR, "Test_Encoder", "SsbSipH264EncodeInit Failed\n");
		return -1;
	}
	// Setting multiple slices
	// This setting must be called before SsbSipH264EncodeExe()
	slices[0] = 0;	// 0 for single, 1 for multiple
	slices[1] = 4;	// count of slices
	SsbSipH264EncodeSetConfig(handle, H264_ENC_SETCONF_NUM_SLICES, slices);
	ret = SsbSipH264EncodeExe(handle);
	*/
	p_inbuf = SsbSipH264EncodeGetInBuf(handle, 0);
	memcpy(p_inbuf, in_addr, frame_length);
	
	ret = SsbSipH264EncodeExe(handle);
	p_outbuf = SsbSipH264EncodeGetOutBuf(handle, &size);//?
	memcpy(out_addr, p_outbuf, size);
	//SsbSipH264EncodeDeInit(handle);
	return size;
	
	
}


	


