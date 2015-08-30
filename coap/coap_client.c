#include "port.h"
#include "ucoap.h"
#include "endpoint.h"

static int __is_socket_inited = 0;

int coap_connect(char * p_str_addr, int port) 
{
	struct hostent * p_host = NULL;
	int unused_endpoint_index = 0;
	coap_endpoint_t * p_endpoint = NULL;
	
	if (0 == __is_socket_inited) 
	{
#ifdef WIN32
		WSADATA wsa;
		BYTE byMajorVersion = 2, byMinorVersion = 2;
		int result = WSAStartup(MAKEWORD(byMajorVersion, byMinorVersion), &wsa);
		if (result != 0 || LOBYTE(wsa.wVersion) != byMajorVersion || HIBYTE(wsa.wVersion) != byMinorVersion) 
		{
			if (result == 0) 
			{
				WSACleanup();
			}
			
			return COAP_RET_SOCKET_INIT_FAILED;
		}
#endif

		coap_init_nodes();

		__is_socket_inited = 1;
	}

	if (p_str_addr == NULL || port < 0 || port > 65535) 
	{
		return COAP_RET_INVALID_PARAMETERS;
	}

	unused_endpoint_index = coap_find_unused_endpoint();
	if (unused_endpoint_index < 0)
	{
		return COAP_RET_NO_MORE_ENDPOINT;
	}
	
	p_host = gethostbyname(p_str_addr);
	if (p_host && phost->h_addr_list[0]) 
	{
		p_endpoint = (coap_endpoint_t *)malloc(sizeof(endpoint));
		p_endpoint->m_servsock = socket(AF_INET, SOCK_DGRAM, 0);
		p_endpoint->m_servaddr.sin_family = AF_INET;
		p_endpoint->m_servaddr.sin_addr.s_addr = *(u_long *)p_host->h_addr_list[0];
		p_endpoint->m_servaddr.sin_port = htons(port);

		coap_set_endpoint(unused_endpoint_index, p_endpoint);

		return unused_endpoint_index;
	}
	else 
	{
		return COAP_RET_UNRESOLVED_HOST;
	}
}

int coap_close(int s)
{
}

int coap_setsockopt(int s, )
{
}

int coap_send(int s, char * p_method, char * p_uri, char * p_data, int len) 
{
	coap_pkt_t * p_pkt = NULL;

	if (0 == strncasecmp(p_method, "GET")    p_pkt = coap_pkt_init(COAP_PKT_TYPE_CON, COAP_PKT_CODE_GET, 0, 1000);
	if (0 == strncasecmp(p_method, "PUT")    p_pkt = coap_pkt_init(COAP_PKT_TYPE_CON, COAP_PKT_CODE_PUT, 0, 1000);
	if (0 == strncasecmp(p_method, "POST")   p_pkt = coap_pkt_init(COAP_PKT_TYPE_CON, COAP_PKT_CODE_POST, 0, 1000);
	if (0 == strncasecmp(p_method, "DELETE") p_pkt = coap_pkt_init(COAP_PKT_TYPE_CON, COAP_PKT_CODE_DELETE, 0, 1000);

	coap_pkt_add_token(p_pkt, NULL, 0);
	coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_URI_PATH, "topic", strlen("topic"));
	coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_URI_QUERY, "root:root", strlen("root:root"));
	coap_pkt_add_data(p_pkt,data, 16);
	
	sendto(fd, (const char *)&p_pkt->hdr, p_pkt->pkt_len, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

}

int coap_recv(int s, char * p_method, char)
{
}
