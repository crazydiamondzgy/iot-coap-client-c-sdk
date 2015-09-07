#include <winsock2.h>
#include <stdio.h>
#include <crtdbg.h>

#include "coap_client.h"

#define DEBUG_NEW new(_NORMAL_BLOCK, __FILE__, __LINE__)
#define new  DEBUG_NEW

int main(int argc, char **argv)
{
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	{
		char buf[2048] = {0};
		int ret = 0;
		int s = coap_connect("115.28.56.102", 5683);
		coap_send(s, "PUT", "/topic?root:root", "hi linuxkernel!", 16);

		while (0 == ret)
		{
			ret = coap_recv(s, buf, 2048);
			Sleep(5000);
		}

		coap_close(s);
	}
	return 0;
}
