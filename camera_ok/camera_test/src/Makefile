TOP_DIR := $(shell pwd)
APP = $(TOP_DIR)/../bin/camera_h264encode
SERVER = $(TOP_DIR)/../bin/rtprecv

CC = arm-linux-gcc
CFLAGS = -g 
LIBS = -lpthread -lx264 -lm 
DEP_LIBS = -L$(TOP_DIR)/../lib
HEADER =
OBJS = main.o video_capture.o h264encoder.o rtp.o HardEncode.o SsbSipH264Encode.o SsbSipLogMsg.o Buffer.o
OBJS_SERVER =  rtpreceive.o

all:  $(OBJS) $(OBJS_SERVER)
	$(CC) -g -o $(APP) $(OBJS) $(LIBS) $(DEP_LIBS) 
	$(CC)  -g -o $(SERVER) $(OBJS_SERVER) $(LIBS) $(DEP_LIBS) 
	cp $(APP) /home/biao/share -rf
	cp $(SERVER) /home/biao/share -rf

clean:
	rm -f *.o a.out $(APP) core *~
