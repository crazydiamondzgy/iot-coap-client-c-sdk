#include <winsock2.h>
#include <stdio.h>
#include "ucoap_cli.h"

int main(int argc, char **argv)
{
    SOCKET fd;
    struct sockaddr_in servaddr;
	struct hostent* server;

#ifdef WIN32
	WSADATA wsa;
	BYTE byMajorVersion = 2, byMinorVersion = 2;
	int result = WSAStartup(MAKEWORD(byMajorVersion, byMinorVersion), &wsa);
	if (result != 0 || LOBYTE(wsa.wVersion) != byMajorVersion || HIBYTE(wsa.wVersion) != byMinorVersion ) 
	{
		if (result == 0) 
		{
			WSACleanup();
		}

		return -1;
	}
#endif

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
	server = gethostbyname("vs0.inf.ethz.ch");
	servaddr.sin_addr.s_addr = *(u_long *)server->h_addr_list[0];
    servaddr.sin_port = htons(5683);

	{
		int len = sizeof(servaddr);
		uint8 buf[512];
		int buflen = 512;
		uint8 data[16] = "I'm linuxkernel";
		struct timeval tv;
		fd_set rfds;
		int ret = 0;

		coap_pkt_t * p_pkt = coap_pkt_init(COAP_PKT_TYPE_CON, COAP_PKT_CODE_GET, 0, 1000);
		coap_pkt_add_token(p_pkt, NULL, 0);
		coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_URI_PATH, ".well-known", strlen(".well-known"));
		coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_URI_PATH, "core", strlen("core"));
		coap_pkt_add_data(p_pkt,data, 16);

		sendto(fd, (const char *)&p_pkt->hdr, p_pkt->pkt_len, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		tv.tv_sec = 10;
		tv.tv_usec = 0;
		FD_ZERO(&rfds);
		FD_SET(fd, &rfds);
		ret = select(0, &rfds, NULL, NULL, &tv);
		if (ret > 0)
		{
			recvfrom(fd, buf, 512, 0,  (struct sockaddr *)&servaddr, &len);
		}

		Sleep(10);
		getchar();
		closesocket(fd);
    }

#ifdef WIN32
	WSACleanup();
#endif

	return 0;
}

