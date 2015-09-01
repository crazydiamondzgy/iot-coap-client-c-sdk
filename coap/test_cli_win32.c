#include <winsock2.h>
#include <stdio.h>
#include "coap_client.h"

int main(int argc, char **argv)
{
	char buf[2048];

	int s = coap_connect("115.28.56.102", 5683);
	coap_send(s, "PUT", "/topic?root:root", "hi linuxkernel!", 16);
	coap_recv(s, buf, 2048);
	coap_close(s);

	return 0;
}
