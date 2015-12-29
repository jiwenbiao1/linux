
#ifndef _HARDENCODE_H
#define _HARDENCODE_H
int HardEncode_test(void* in_addr,size_t frame_length,void* out_addr,int width, int height, int frame_rate, int bitrate, int gop_num);
void HardEncode_destory();
int HardEncode_init(int width, int height);
#endif
