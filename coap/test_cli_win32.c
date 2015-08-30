#include <winsock2.h>
#include <stdio.h>
#include "ucoap.h"

int main(int argc, char **argv)
{

		int len = sizeof(servaddr);
		uint8 buf[512];
		int buflen = 512;
		uint8 data[16] = "I'm linuxkernel";
		struct timeval tv;
		fd_set rfds;
		int ret = 0;
		
		while (1)
		{
			tv.tv_sec = 1;
			tv.tv_usec = 0;
			FD_ZERO(&rfds);
			FD_SET(fd, &rfds);
			ret = select(0, &rfds, NULL, NULL, &tv);
			if (ret > 0)
			{
				ret = recvfrom(fd, buf, 512, 0,  (struct sockaddr *)&servaddr, &len);
			}
			else if (ret == 0)
			{
				break;
			}
		}
		
		getchar();
		closesocket(fd);


	return 0;
}
