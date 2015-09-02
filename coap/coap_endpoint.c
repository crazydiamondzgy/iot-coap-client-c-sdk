#include <assert.h>

#include "coap_endpoint.h"
#include "coap_log.h"
#include "coap_util.h"
#include "coap_packet.h"

void coap_check_timeouts()
{
}

int coap_is_valid_packet(coap_pkt_t * p_pkt)
{
	return 1;
}

#ifndef _WIN32
void * coap_work_thread(void * p_void)
#else
DWORD WINAPI coap_work_thread(LPVOID p_void)
#endif
{
	coap_endpoint_mgr_t * p_endpoint_mgr = (coap_endpoint_mgr_t *)p_void;

	char buf[COAP_MAX_PKT_SIZE];
	struct sockaddr_in sockaddr;
	int socklen = sizeof(sockaddr);
	int ret = 0;
	int len = 0;
	fd_set rfds;

	while (1 == p_endpoint_mgr->m_endpoint_mgr_is_running) 
	{
		socket_t max_sock = 0;
		int i = 0;
		struct timeval tv;
		p_endpoint_mgr->m_endpoint_mgr_time_cache = coap_get_milliseconds();

		coap_check_timeouts();

		for (FD_ZERO(&rfds); i < COAP_MAX_ENDPOINTS; i++)
		{
			coap_endpoint_t * p_endpoint = coap_get_endpoint(p_endpoint_mgr, i);
			if (p_endpoint)
			{
				FD_SET(p_endpoint->m_servsock, &rfds);
				max_sock = max(max_sock, p_endpoint->m_servsock);
			}
		}
		
		if (max_sock == 0)
		{
			continue;
		}

		tv.tv_sec = 0;
		tv.tv_usec = 500000;
		ret = select(max_sock + 1, &rfds, NULL, NULL, &tv);
		if (ret < 0) 
		{
			coap_log_error_string("## socket error, please restart coap\r\n");

			break;
		}
		else if (0 == ret) 
		{
			continue;
		}
		else
		{
			for (i = 0; i < COAP_MAX_ENDPOINTS; i++)
			{
				coap_endpoint_t * p_endpoint = coap_get_endpoint(p_endpoint_mgr, i);

				if (FD_ISSET(p_endpoint->m_servsock, &rfds))
				{
					coap_pkt_t * p_pkt = (coap_pkt_t *)buf;
					memset(buf, 0, COAP_MAX_PKT_SIZE);
					memset(&sockaddr, 0, sizeof(sockaddr));
					
					len = recvfrom(p_endpoint->m_servsock, (char *)buf, COAP_MAX_PKT_SIZE, 0, (struct sockaddr *)&sockaddr, (socklen_t *)&socklen);
					if (len <= 0) 
					{
						int err = coap_get_error_code();
						coap_log_error_string("Socket Error:%d!\r\n", err);
						continue;
					}
					
					if (0 == coap_is_valid_packet(p_pkt)) 
					{
						continue;
					}
					
					coap_log_debug_string("## Incorrect Packet\r\n");
					coap_log_debug_binary(buf, len);
				}
			}
		}
	}

	coap_log_error_string("## coap main thread exited\r\n");

#ifndef _WIN32
	return NULL;
#else
	return 0;
#endif
}

int coap_clear_endpoints(coap_endpoint_mgr_t * p_endpoint_mgr)
{
	assert(p_endpoint_mgr);

	memset(p_endpoint_mgr->m_coap_endpoints, 0, sizeof(coap_endpoint_t)*COAP_MAX_ENDPOINTS);

	return 0;
}

int coap_init_endpoints(coap_endpoint_mgr_t * p_endpoint_mgr)
{
	assert(p_endpoint_mgr);

	coap_clear_endpoints(p_endpoint_mgr);

	p_endpoint_mgr->m_endpoint_mgr_is_running = 1;
	
#ifndef _WIN32
	pthread_create(&p_endpoint_mgr->m_endpoint_mgr_thread, NULL, coap_work_thread, p_endpoint_mgr);
#else
	p_endpoint_mgr->m_endpoint_mgr_thread = CreateThread(NULL, 0, coap_work_thread, p_endpoint_mgr, 0, NULL);
#endif

	return 0;
}

int coap_find_unused_endpoint(coap_endpoint_mgr_t * p_endpoint_mgr)
{
	int i = 0;

	assert(p_endpoint_mgr);

	for (; i < COAP_MAX_ENDPOINTS; i++)
	{
		if (p_endpoint_mgr->m_coap_endpoints[i])
		{
			return i;
		}
	}

	return -1;
}

coap_endpoint_t * coap_get_endpoint(coap_endpoint_mgr_t * p_endpoint_mgr, int i)
{
	assert(p_endpoint_mgr);

	if (i < COAP_MAX_ENDPOINTS && i > 0)
	{
		return p_endpoint_mgr->m_coap_endpoints[i];
	}

	return NULL;
}

int coap_set_endpoint(coap_endpoint_mgr_t * p_endpoint_mgr, int i, coap_endpoint_t * p_endpoint)
{
	assert(p_endpoint_mgr);

	if (i < COAP_MAX_ENDPOINTS && i > 0)
	{
		assert(p_endpoint_mgr->m_coap_endpoints[i] == NULL);
		p_endpoint_mgr->m_coap_endpoints[i] = p_endpoint;
	}

	return -1;
}
