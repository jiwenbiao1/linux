//send.c
      #include <stdio.h>
      #include <string.h>
      #include <malloc.h>
      #include <sys/types.h>
      #include <sys/stat.h>
      #include <fcntl.h>
      #include <unistd.h>
      #include <termios.h>

      #define MAX_BUFFER_SIZE 512

      int fd, flag_close;

	  int createpackage();
      int open_serial()
      {
            //这里的/dev/pts/1是使用mkptych.py虚拟的两个串口名字之一
            fd = open("/dev/pty6", O_RDWR | O_NOCTTY | O_NONBLOCK);
            if(fd == -1)
            {
                  perror("open serial port error!\n");
                  return -1;
            }

            printf("Open serial port success!");
            return 0;
      }

	 int package_count;
	 int count;
	 unsigned char* sbuf;
      int main(int argc, char* argv[])
      {
      	
            int retv;
            struct termios option;
            retv = open_serial();
            if(retv < 0)
            {
                  perror("open serial port error!\n");
                  return -1;
            }

            printf("Ready for sending data...\n");

            tcgetattr(fd, &option);
            cfmakeraw(&option);
            cfsetispeed(&option, B9600);
            cfsetospeed(&option, B9600);
            tcsetattr(fd, TCSANOW, &option);
	 
			while(1)
				{
				

				 	createpackage();
		         	retv = write(fd, sbuf, package_count);
		            if(retv == -1)
		            {
		                  perror("Write data error!\n");
		                  return -1;
		            }
		            printf("The number of char sent is %d\n", retv);
					//sleep(1);
				}
            
            return 0;
      }


int createpackage()
{
			printf("input count \t");
			scanf("%d",&count);
			printf("count =%d\n",count);
			package_count = sizeof(unsigned char)*(count+3+1+1);
			if((sbuf = (unsigned char*) malloc(package_count))==NULL)
				perror("malloc sbuf");
			memset(sbuf,0,package_count);
           	sbuf[0]='\xFF';
			sbuf[1]='\xFF';
			sbuf[2]='\xAA';
			sbuf[3]=(unsigned char)count;
			//printf("num= %.2X \n",(unsigned char)sbuf[3]);
			int sum =0;
			for(int i=0;i<sbuf[3];i++)
				{
					sbuf[4+i] = (unsigned char)i;
					sum+=i;
				}
			unsigned char cks = (unsigned char)(0x100 - sum);
			sbuf[package_count-1]=cks;
			for(int i=0;i<package_count;i++)
				{
					printf("%.2X ",(unsigned char)sbuf[i]);
				}
			printf("\n");
}
