//receive.c
    #include <stdio.h>
	
    #include <string.h>
    #include <malloc.h>
    #include <sys/types.h>
    #include <sys/stat.h>
    #include <fcntl.h>
    #include <unistd.h>
    #include <termios.h>
    #include <math.h>
	#include "queue.h"
    #define MAX_BUFFER_SIZE 512

    int fd, s;

    int open_serial()
    {
          //这里的/dev/pts/2是使用mkptych.py虚拟的两个串口名字之一
          fd = open("/dev/pty7", O_RDWR|O_NOCTTY|O_NDELAY);
          if(fd == -1)
          {
                perror("open serial port error!\n");
                return -1;
          }

          printf("open /dev/pts/3.\n");
          return 0;
    }

    int main()
    {
          char hd[MAX_BUFFER_SIZE], *rbuf;
          int flag_close, retv=0;
          struct termios opt;
		  Queue buf_queue;
		  InitQueue(&buf_queue);
          retv = open_serial();
          if(retv < 0)
          {
                printf("Open serrial port error!\n");
                return -1;
          }

          tcgetattr(fd, &opt);//绑定文件描述符和termios
	            cfmakeraw(&opt);//原始模式
	            cfsetispeed(&opt, B9600);//输入速率
	            cfsetospeed(&opt, B9600);//输出速率
          tcsetattr(fd, TCSANOW, &opt);//更新终端参数   使参数生效
          rbuf = hd;
          printf("Ready for receiving data...\n");
		  unsigned char* buf;
		  if((buf = (char*)malloc(sizeof(unsigned char)*512))==NULL)
		  	perror("malloc buf ");
		  memset(buf,0,512);
		  char package_head[3]={'\xff','\xff','\xaa'};
			if((retv = read(fd, rbuf, 300)) > 0)
				{
					printf( "read count = %d \n",retv);
					InQueue(&buf_queue,rbuf,retv);
				}
			
			OutQueue(&buf_queue,buf,1);
			
          while(1)
          	{
          		if((retv = read(fd, rbuf, 300)) > 0)
				{
					printf( "read count = %d \n",retv);
					InQueue(&buf_queue,rbuf,retv);
				}
			
          		 
				//LookQueue(&buf_queue);
				//printf("front = %d\t rear = %d\n",buf_queue.front,buf_queue.rear);
				//OutQueue(&buf_queue,buf,retv);
				//LookQueue(&buf_queue);
				//printf("front = %d\t rear = %d\n",buf_queue.front,buf_queue.rear);
		
				if(OutQueue(&buf_queue,buf,3)==3)
					{

		lab1:				if(memcmp(buf,package_head,3)==0)
							{
								//比对成功 one package
								OutQueue(&buf_queue,buf,1);
								int num = *buf;
								printf("packet size = %.2x\n",num);
								OutQueue(&buf_queue,buf,num+1);
								int i=0,sum=0;
								while(i<num)
								{
									printf("%.2x \t",buf[i]);
									sum+=buf[i++];
								}
								
								printf("\n");
								if(buf[num]==(unsigned char)(0x100-sum))
									printf("check correct \n");
								memset(buf,0,512);
								
							}
						else
							{
								
								 buf[0]=buf[1];
								 buf[1]=buf[2];
								 
								 OutQueue(&buf_queue,buf+2,1);
								 goto lab1;
							}
						
					}
				
				
				sleep(1);
				
          	}
		
				
				
                     
       

          printf("\n");
          flag_close = close(fd);
          if(flag_close == -1)
                printf("Close the device failure!\n");

          return 0;
    }

