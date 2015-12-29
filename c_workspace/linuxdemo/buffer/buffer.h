/*
 * buffer.h
 *
 *  Created on: 2015Äê12ÔÂ27ÈÕ
 *      Author: biao
 */

#ifndef BUFFER_BUFFER_H_
#define BUFFER_BUFFER_H_

#define YUV_FRAME_SIZE 640*480*5
typedef unsigned char uint8_t;

typedef struct   {
	uint8_t buf[YUV_FRAME_SIZE];
	int length;
}capture_buffer;




#endif /* BUFFER_BUFFER_H_ */
