#CROSS:=arm-hisiv100nptl-linux-
CC:=$(CROSS)g++ -g

AR:=$(CROSS)ar
STRIP:=$(CROSS)strip

SERVER_NAME:= server_rtsp

SERVER_OBJS = Socket.o Udp.o Tcp.o Rtp.o Rtsp.o MediaStream.o MediaStreamH264.o \
	 RtspResponse.o RtspSession.o RtspTransport.o RtspService_t.o RtspService.o

all: server

server: $(SERVER_OBJS) 
	$(CC) -o $(SERVER_NAME) $(SERVER_OBJS) -lpthread
#	$(STRIP) $(SERVER_NAME)

clean:
	rm -f *.o *.a $(SERVER_NAME)


