#include <asm/types.h>          /* for videodev2.h */
#include <fcntl.h>              /* low-level i/o */
#include <unistd.h>
#include <errno.h>
#include <malloc.h>
#include <sys/stat.h>
#include <sys/types.h>
//#include <sys/time.h>
#include <time.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <linux/videodev2.h>
#include <dirent.h>
#include "video_capture.h"
#include "h264encoder.h"
#include "rtp.h"
#define CLEAR(x) memset (&(x), 0, sizeof (x))

typedef unsigned char uint8_t;

static char *dev_name = "/dev/video0";

char h264_file_name[100] = "zgy.h264\0";
FILE *h264_fp;
uint8_t *h264_buf;

char yuv_file_name[100] = "zgy.yuv\0";
FILE *yuv_fp;

unsigned int n_buffers = 0;
DIR *dirp;
Encoder en;

int cnt = 0;

void errno_exit(const char *s) {
	fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
	exit(EXIT_FAILURE);
}

int xioctl(int fd, int request, void *arg) {
	int r = 0;
	do {
		r = ioctl(fd, request, arg);
	} while (-1 == r && EINTR == errno);

	return r;
}

void open_camera(struct camera *cam) {
	struct stat st;

	if (-1 == stat(cam->device_name, &st)) {
		fprintf(stderr, "Cannot identify '%s': %d, %s\n", cam->device_name,
				errno, strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (!S_ISCHR(st.st_mode)) {
		fprintf(stderr, "%s is no device\n", cam->device_name);
		exit(EXIT_FAILURE);
	}

	cam->fd = open(cam->device_name, O_RDWR, 0); //  | O_NONBLOCK

	if (-1 == cam->fd) {
		fprintf(stderr, "Cannot open '%s': %d, %s\n", cam->device_name, errno,
				strerror(errno));
		exit(EXIT_FAILURE);
	}
}

void close_camera(struct camera *cam) {
	if (-1 == close(cam->fd))
		errno_exit("close");

	cam->fd = -1;
}

void init_file() {
	h264_fp = fopen(h264_file_name, "wa+");
	yuv_fp = fopen(yuv_file_name, "wa+");
}

void close_file() {
	fclose(h264_fp);
	fclose(yuv_fp);
}

void init_encoder(struct camera *cam) {
	compress_begin(&en, cam->width, cam->height);
	h264_buf = (uint8_t *) malloc(
			sizeof(uint8_t) * cam->width * cam->height * 3); // è®¾ç½®ç¼“å†²åŒº
}

void close_encoder() {
	compress_end(&en);
	free(h264_buf);
}
extern void sendh264frame(uint8_t *h264_buf, size_t h264_length);
extern int HardEncode_test(void* in_addr,size_t frame_length,void* out_addr,int width, int height, int frame_rate, int bitrate, int gop_num);
static int YUV422T0420(unsigned char* pYUV, unsigned char* yuv, int lWidth, int lHeight)
{
	int i, j;
	unsigned char* pY = yuv;
	unsigned char* pU = yuv + lWidth * lHeight;
	unsigned char* pV = pU + (lWidth * lHeight) / 4;

	unsigned char* pYUVTemp = pYUV;
	unsigned char* pYUVTempNext = pYUV + lWidth * 2;

	for (i = 0; i < lHeight; i+= 2)
	{
		for (j = 0; j < lWidth; j+=2)
		{
			pY[j] = *pYUVTemp++;
			pY[j+lWidth] = *pYUVTempNext++;

			pU[j/2] = (*(pYUVTemp) + *(pYUVTempNext)) / 2;
			pYUVTemp++;
			pYUVTempNext++;

			pY[j+1] = *pYUVTemp++;
			pY[j+1+lWidth] = *pYUVTempNext++;

			pV[j/2] = (*(pYUVTemp) + *(pYUVTempNext)) / 2;
			pYUVTemp++;
			pYUVTempNext++;
		}
		pYUVTemp += lWidth * 2;
		pYUVTempNext += lWidth * 2;

		pY += lWidth * 2;
		pU += lWidth / 2;
		pV += lWidth / 2;
	}
	return 1;
}


void encode_frame(struct camera *cam,uint8_t *yuv_frame, size_t yuv_length) {
	int h264_length = 0;

	//è¿™é‡Œæœ‰ä¸€ä¸ªé—®é¢˜ï¼Œé€šè¿‡æµ‹è¯•å‘çŽ°å‰6å¸§éƒ½æ˜¯0ï¼Œæ‰€ä»¥è¿™é‡Œæˆ‘è·³è¿‡äº†ä¸º0çš„å¸§
	if (yuv_frame[0] == '\0')
		return;
	unsigned char* yuv;
	if((yuv=(unsigned char*)malloc(sizeof(unsigned char)*cam->width*cam->height*3/2))==NULL)
		{
			perror("malloc");
		}
	//Ó²¼þ±àÂë
	YUV422T0420( yuv_frame,\
				 yuv,\
				 cam->width,\
				 cam->height);
	h264_length =  HardEncode_test( yuv,\
									 cam->width*cam->height*3/2,\
									 h264_buf,\
									cam->width,\
									cam->height,\
									0,\
									0,\
									0);
	//Èí¼þ±àÂë
	//h264_length = compress_frame(&en, -1, yuv_frame, h264_buf);
	if (h264_length > 0) {
		//å†™h264æ–‡ä»¶
		fwrite(h264_buf, h264_length, 1, h264_fp);
		
		sendh264frame(h264_buf, h264_length);
		
		
		
	}

	free(yuv);
	//å†™yuvæ–‡ä»¶
	//fwrite(yuv_frame, yuv_length, 1, yuv_fp);
}
extern struct camera *cam;

void encod_from_buffer(void)
{
	void* out;
	int length;
	while(1)
	{
		consume(&out,&length);
		encode_frame(cam,out,length);
	}

}

int read_and_encode_frame(struct camera *cam) {
	struct v4l2_buffer buf;

	//printf("in read_frame\n");

	CLEAR(buf);

	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	//this operator below will change buf.index and (0 <= buf.index <= 3)
	if (-1 == xioctl(cam->fd, VIDIOC_DQBUF, &buf)) {//Êý¾Ý³ö¶ÓÁÐ
		switch (errno) {
		case EAGAIN:
			return 0;
		case EIO:
			/* Could ignore EIO, see spec. */
			/* fall through */
		default:
			errno_exit("VIDIOC_DQBUF");
		}
	}

		/*
			 1 ²É¼¯³öÀ´µÄÊý¾ÝÖ¡´æ·ÅÔÚbufferÖÐ  ¿ªÊ¼µØÖ·  ºÍ³¤¶È 
			 2  ´ÓbufferÖÐÈ¡µÃÊý¾Ý²¢±àÂë	
			 
		*/
		//11²É¼¯²¢´æ·Åµ½»º³åÖÐ 
		product(cam->buffers[buf.index].start,buf.length);
		//encode_frame(cam,cam->buffers[buf.index].start, buf.length);

	if (-1 == xioctl(cam->fd, VIDIOC_QBUF, &buf))//Ö¡Èë¶ÓÁÐ ÔÙ´ÎÓÃÀ´´æ·ÅÊý¾Ý
		errno_exit("VIDIOC_QBUF");

	return 1;
}

void start_capturing(struct camera *cam) {
	unsigned int i;
	enum v4l2_buf_type type;

	for (i = 0; i < n_buffers; ++i) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (-1 == xioctl(cam->fd, VIDIOC_QBUF, &buf))
			errno_exit("VIDIOC_QBUF");
	}

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(cam->fd, VIDIOC_STREAMON, &type))
		errno_exit("VIDIOC_STREAMON");

}

void stop_capturing(struct camera *cam) {
	enum v4l2_buf_type type;

	type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (-1 == xioctl(cam->fd, VIDIOC_STREAMOFF, &type))
		errno_exit("VIDIOC_STREAMOFF");

}
void uninit_camera(struct camera *cam) {
	unsigned int i;

	for (i = 0; i < n_buffers; ++i)
		if (-1 == munmap(cam->buffers[i].start, cam->buffers[i].length))
			errno_exit("munmap");

	free(cam->buffers);
}

void init_mmap(struct camera *cam) {
	struct v4l2_requestbuffers req;

	CLEAR(req);

	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;

	//åˆ†é…å†…å­˜
	if (-1 == xioctl(cam->fd, VIDIOC_REQBUFS, &req)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s does not support "
					"memory mapping\n", cam->device_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_REQBUFS");
		}
	}

	if (req.count < 2) {
		fprintf(stderr, "Insufficient buffer memory on %s\n", cam->device_name);
		exit(EXIT_FAILURE);
	}

	cam->buffers = calloc(req.count, sizeof(*(cam->buffers)));

	if (!cam->buffers) {
		fprintf(stderr, "Out of memory\n");
		exit(EXIT_FAILURE);
	}

	for (n_buffers = 0; n_buffers < req.count; ++n_buffers) {
		struct v4l2_buffer buf;

		CLEAR(buf);

		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = n_buffers;

		//å°†VIDIOC_REQBUFSä¸­åˆ†é…çš„æ•°æ®ç¼“å­˜è½¬æ¢æˆç‰©ç†åœ°å€
		if (-1 == xioctl(cam->fd, VIDIOC_QUERYBUF, &buf))
			errno_exit("VIDIOC_QUERYBUF");

		cam->buffers[n_buffers].length = buf.length;
		cam->buffers[n_buffers].start = mmap(NULL /* start anywhere */,
				buf.length, PROT_READ | PROT_WRITE /* required */,
				MAP_SHARED /* recommended */, cam->fd, buf.m.offset);

		if (MAP_FAILED == cam->buffers[n_buffers].start)
			errno_exit("mmap");
	}
}

void init_camera(struct camera *cam) {
	struct v4l2_capability *cap = &(cam->v4l2_cap);
	struct v4l2_cropcap *cropcap = &(cam->v4l2_cropcap);
	struct v4l2_crop *crop = &(cam->crop);
	struct v4l2_format *fmt = &(cam->v4l2_fmt);
	unsigned int min;

	if (-1 == xioctl(cam->fd, VIDIOC_QUERYCAP, cap)) {
		if (EINVAL == errno) {
			fprintf(stderr, "%s is no V4L2 device\n", cam->device_name);
			exit(EXIT_FAILURE);
		} else {
			errno_exit("VIDIOC_QUERYCAP");
		}
	}

	if (!(cap->capabilities & V4L2_CAP_VIDEO_CAPTURE)) {
		fprintf(stderr, "%s is no video capture device\n", cam->device_name);
		exit(EXIT_FAILURE);
	}

	if (!(cap->capabilities & V4L2_CAP_STREAMING)) {
		fprintf(stderr, "%s does not support streaming i/o\n",
				cam->device_name);
		exit(EXIT_FAILURE);
	}

	//#ifdef DEBUG_CAM
	printf("\nVIDOOC_QUERYCAP\n");
	printf("the camera driver is %s\n", cap->driver);
	printf("the camera card is %s\n", cap->card);
	printf("the camera bus info is %s\n", cap->bus_info);
	printf("the version is %d\n", cap->version);
	//#endif
	/* Select video input, video standard and tune here. */

	CLEAR(*cropcap);

	cropcap->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	crop->c.width = cam->width;
	crop->c.height = cam->height;
	crop->c.left = 0;
	crop->c.top = 0;
	crop->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	CLEAR(*fmt);

	fmt->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt->fmt.pix.width = cam->width;
	fmt->fmt.pix.height = cam->height;
	//fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_MJPEG;
	fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; //yuv422
	//  fmt->fmt.pix.pixelformat = V4L2_PIX_FMT_YUV420  //yuv420 ä½†æ˜¯æˆ‘ç”µè„‘ä¸æ”¯æŒ
	fmt->fmt.pix.field = V4L2_FIELD_INTERLACED; //éš”è¡Œæ‰«æ

	if (-1 == xioctl(cam->fd, VIDIOC_S_FMT, fmt))
		errno_exit("VIDIOC_S_FMT");

	/* Note VIDIOC_S_FMT may change width and height. */

	/* Buggy driver paranoia. */
	min = fmt->fmt.pix.width * 2;
	if (fmt->fmt.pix.bytesperline < min)
		fmt->fmt.pix.bytesperline = min;
	min = fmt->fmt.pix.bytesperline * fmt->fmt.pix.height;
	if (fmt->fmt.pix.sizeimage < min)
		fmt->fmt.pix.sizeimage = min;

	init_mmap(cam);

}

void v4l2_init(struct camera *cam) {
	open_camera(cam);
	init_camera(cam);
	start_capturing(cam);
	//Ó²¼þ±àÂëÆ÷
	HardEncode_init(cam->width,cam->height);
	//Èí¼þ±àÂë
	init_encoder(cam);
	init_file();
}

void v4l2_close(struct camera *cam) {
	stop_capturing(cam);
	uninit_camera(cam);
	close_camera(cam);
	free(cam);
	close_file();
	close_encoder();
}

