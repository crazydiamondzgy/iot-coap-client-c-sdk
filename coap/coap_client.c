#include "os_support.h"
#include "coap_log.h"
#include "coap_util.h"
#include "coap_client.h"
#include "coap_endpoint.h"
#include "coap_packet.h"

static int __is_socket_inited = 0;
static coap_endpoint_mgr_t __coap_endpoint_mgr;

int coap_setsockopt(int s, int level, int optname, void * optval, int optlen)
{
	return 0;
}

int coap_getsockopt(int s, int level, int optname, void * optval)
{
	return 0;
}

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

		coap_init_endpoints(&__coap_endpoint_mgr);

		__is_socket_inited = 1;
	}

	if (p_str_addr == NULL || port < 0 || port > 65535) 
	{
		return COAP_RET_INVALID_PARAMETERS;
	}

	unused_endpoint_index = coap_find_unused_endpoint(&__coap_endpoint_mgr);
	if (unused_endpoint_index < 0)
	{
		return COAP_RET_NO_MORE_NODE;
	}
	
	p_host = gethostbyname(p_str_addr);
	if (p_host && p_host->h_addr_list[0]) 
	{
		p_endpoint = coap_alloc_endpoint();
		p_endpoint->m_servsock = socket(AF_INET, SOCK_DGRAM, 0);
		p_endpoint->m_servaddr.sin_family = AF_INET;
		p_endpoint->m_servaddr.sin_addr.s_addr = *(u_long *)p_host->h_addr_list[0];
		p_endpoint->m_servaddr.sin_port = htons((int16)port);
		
		coap_set_endpoint(&__coap_endpoint_mgr, unused_endpoint_index, p_endpoint);

		return unused_endpoint_index;
	}
	else 
	{
		return COAP_RET_UNRESOLVED_HOST;
	}
}

int coap_close(int s)
{
	int ret = 0;
	
	ret = coap_free_endpoint(&__coap_endpoint_mgr, s);
	if (ret < 0)
	{
		return ret;
	}

	ret = coap_set_endpoint(&__coap_endpoint_mgr, s, NULL);
	if (ret < 0)
	{
		return ret;
	}

	return 0;
}

int coap_send(int s, char * p_method, char * p_url, char * p_data, int len) 
{
	char * p_path = p_url;
	char * p_query = strchr(p_url, '?') ? strchr(p_url, '?') : p_url + strlen(p_url);
	char * p_end = p_url + strlen(p_url);
	char * p = NULL;
	coap_pkt_t * p_pkt = NULL;
	coap_endpoint_t * p_endpoint = coap_get_endpoint(&__coap_endpoint_mgr, s);
	if (NULL == p_endpoint)
	{
		return COAP_RET_INVALID_PARAMETERS;
	}

	if (0 == strncasecmp(p_method, "GET", 4))
	{
		p_pkt = coap_pkt_alloc(COAP_MAX_PKT_SIZE);
		coap_pkt_add_header(p_pkt, COAP_PKT_TYPE_CON, COAP_PKT_CODE_GET, 0);
	}

	if (0 == strncasecmp(p_method, "PUT", 4))
	{
		p_pkt = coap_pkt_alloc(COAP_MAX_PKT_SIZE);
		coap_pkt_add_header(p_pkt, COAP_PKT_TYPE_CON, COAP_PKT_CODE_PUT, 0);
	}

	if (0 == strncasecmp(p_method, "POST", 4))
	{
		p_pkt = coap_pkt_alloc(COAP_MAX_PKT_SIZE);
		coap_pkt_add_header(p_pkt, COAP_PKT_TYPE_CON, COAP_PKT_CODE_POST, 0);
	}

	if (0 == strncasecmp(p_method, "DELETE", 7))
	{
		p_pkt = coap_pkt_alloc(COAP_MAX_PKT_SIZE);
		coap_pkt_add_header(p_pkt, COAP_PKT_TYPE_CON, COAP_PKT_CODE_DELETE, 0);
	}

	if (NULL == p_pkt)
	{
		coap_log_error_string("## unsupported method!\r\n");
		return COAP_RET_INVALID_PARAMETERS;
	}

	coap_pkt_add_token(p_pkt, NULL, 0);

	for (p=p_path; p<=p_query; p++)
	{
		if (*p == '/' || p == p_query)
		{
			coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_URI_PATH, p_path, p - p_path);
			p_path = p + 1;
		}
	}

	for (++p_query; p<=p_end; p++)
	{
		if (*p == '&' || p == p_end)
		{
			coap_pkt_add_option(p_pkt, COAP_PKT_OPTION_URI_QUERY, p_query, p - p_query);
			p_query = p + 1;
		}
	}
	
	coap_pkt_add_data(p_pkt, p_data, len);
	
	p_pkt->last_transmit_time = coap_get_milliseconds();
	p_pkt->retransmit_count = 0;
	p_pkt->retransmit_interval = COAP_ACK_TIMEOUT;

	mutex_lock(&p_endpoint->m_send_mutex);
	coap_send_queue_push_node(p_endpoint, p_pkt);
	mutex_unlock(&p_endpoint->m_send_mutex);
	
	sendto(p_endpoint->m_servsock, (const char *)&p_pkt->hdr, p_pkt->len, 0, (struct sockaddr *)&p_endpoint->m_servaddr, sizeof(p_endpoint->m_servaddr));

	return 0;
}

int coap_recv(int s, char * p_data, int len)
{
	int ret = 0;
	coap_endpoint_t * p_endpoint = coap_get_endpoint(&__coap_endpoint_mgr, s);
	if (NULL == p_endpoint)
	{
		return COAP_RET_INVALID_PARAMETERS;
	}

	mutex_lock(&p_endpoint->m_recv_mutex);
	ret = coap_recv_queue_pop_node(p_endpoint, p_data, len);
	mutex_unlock(&p_endpoint->m_recv_mutex);

	return ret;
}
