#include "rtp.h"

int main(int argc, char** argv)
{
    rtp_s input;
    char ip[40] = "192.168.169.101";
    rtp_init(&input, ip, 1234);

    while (1)
    {
        sleep(10);
    }
    return 0;
}
