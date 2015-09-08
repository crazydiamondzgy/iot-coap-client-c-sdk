#include <winsock2.h>
#include <stdio.h>

#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h>
#include <crtdbg.h>

#include "ucoap.h"

int main(int argc, char **argv)
{
	char buf[2048] = {0};
	char * data = "hi linuxkernel!";
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	{
		int ret = 0;
		int s = coap_connect("115.28.56.102", 5683);
		coap_send(s, "PUT", "/topic?root:root", data, strlen(data));

		while (0 == ret)
		{
			ret = coap_recv(s, buf, 2048);
			Sleep(5000);
		}

		coap_close(s);
	}
	return 0;
}
