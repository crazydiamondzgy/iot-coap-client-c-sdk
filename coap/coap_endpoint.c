#ifdef _WIN32
#include <winsock2.h>
#endif

#include <assert.h>

#include "queue.h"
#include "mutex.h"
#include "coap_log.h"
#include "coap_util.h"
#include "coap_packet.h"
#include "coap_endpoint.h"
#include "coap_client.h"

coap_endpoint_t * coap_alloc_endpoint(coap_endpoint_mgr_t * p_endpoint_mgr)
{
	coap_endpoint_t * p_endpoint = (coap_endpoint_t *)malloc(sizeof(coap_endpoint_t));
	
	queue_init(&p_endpoint->m_send_queue);
	queue_init(&p_endpoint->m_recv_queue);
	mutex_init(&p_endpoint->m_send_mutex);
	mutex_init(&p_endpoint->m_recv_mutex);
	
	return p_endpoint;
}

int coap_free_endpoint(coap_endpoint_mgr_t * p_endpoint_mgr, int i)
{
	coap_pkt_t * p_pkt = NULL;
	
	coap_endpoint_t * p_endpoint = coap_get_endpoint(p_endpoint_mgr, i);
	if (NULL == p_endpoint)
	{
		return COAP_RET_INVALID_PARAMETERS;
	}

	while (!queue_is_empty(&p_endpoint->m_send_queue)) 
	{
		p_pkt = queue_entry(p_endpoint->m_send_queue.next, coap_pkt_t, node);
		queue_del(&p_pkt->node);
		free(p_pkt);
	}
	
	while (!queue_is_empty(&p_endpoint->m_recv_queue)) 
	{
		p_pkt = queue_entry(p_endpoint->m_recv_queue.next, coap_pkt_t, node);
		queue_del(&p_pkt->node);
		free(p_pkt);
	}

	mutex_uninit(&p_endpoint->m_send_mutex);
	mutex_uninit(&p_endpoint->m_recv_mutex);

	return 0;
}

void coap_check_timeouts(coap_endpoint_mgr_t * p_endpoint_mgr, int i)
{
	struct queue_hdr_t *p = NULL, *next = NULL;
	coap_endpoint_t * p_endpoint = coap_get_endpoint(p_endpoint_mgr, i);
	if (NULL == p_endpoint)
	{
		return;
	}

	for (p = p_endpoint->m_send_queue.next; p != &p_endpoint->m_send_queue; p = next)
	{
		coap_pkt_t * p_pkt = queue_entry(p, coap_pkt_t, node);
		next = p->next;
		if (p_pkt->last_transmit_time + p_pkt->retransmit_interval > p_endpoint_mgr->m_endpoint_mgr_time_cache) 
		{
			if (p_pkt->retransmit_count < COAP_MAX_RETRANSMIT)
			{
				sendto(p_endpoint->m_servsock, (const char *)&p_pkt->hdr, p_pkt->len, 0, (struct sockaddr *)&p_endpoint->m_servaddr, sizeof(p_endpoint->m_servaddr));
				p_pkt->last_transmit_time = coap_get_milliseconds();
				p_pkt->retransmit_interval <<= 1;
				p_pkt->retransmit_count++;
			}
			else
			{
				coap_free_endpoint(p_endpoint_mgr, i);
				coap_set_endpoint(p_endpoint_mgr, i, NULL);
				break;
			}
		}
	}

	return;
}

void coap_dispatch(coap_endpoint_t * p_endpoint, char * buf, int len) 
{
	coap_hdr_t * p_hdr = (coap_hdr_t *)buf;
	assert(p_endpoint);
	
    switch (p_hdr->type)
	{
    case COAP_PKT_TYPE_ACK:
		coap_send_queue_del_node(p_endpoint, p_hdr->message_id);
		if (p_hdr->code == 0) goto cleanup;
		break;
		
    case COAP_PKT_TYPE_RST:
		coap_send_queue_del_node(p_endpoint, p_hdr->message_id);
		goto cleanup;
		
    case COAP_PKT_TYPE_NON:
		break;
		
    case COAP_PKT_TYPE_CON :
		break;
    default: 
		break;
    }
	
	if (!COAP_PKT_IS_EMPTY(p_hdr))
	{
		coap_pkt_t * p_pkt = coap_pkt_alloc(len);
		p_pkt->len = len;
		memcpy(&p_pkt->hdr, buf, len);
		
		mutex_lock(&p_endpoint->m_recv_mutex);
		coap_recv_queue_push_node(p_endpoint, p_pkt);
		mutex_unlock(&p_endpoint->m_recv_mutex);
	}
    
	return;

cleanup:
    return;
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

		for (i = 0; i < COAP_MAX_ENDPOINTS; i++)
		{
			coap_check_timeouts(p_endpoint_mgr, i);
		}

		for (i = 0, FD_ZERO(&rfds); i < COAP_MAX_ENDPOINTS; i++)
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
					memset(buf, 0, COAP_MAX_PKT_SIZE);
					memset(&sockaddr, 0, sizeof(sockaddr));
					
					len = recvfrom(p_endpoint->m_servsock, (char *)buf, COAP_MAX_PKT_SIZE, 0, (struct sockaddr *)&sockaddr, (socklen_t *)&socklen);
					if (len <= 0) 
					{
						int err = coap_get_error_code();
						coap_log_error_string("Socket Error:%d!\r\n", err);
						continue;
					}
					
					coap_dispatch(p_endpoint, buf, len);
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
	int i = 0;
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

int coap_send_queue_push_node(coap_endpoint_t * p_endpoint, coap_pkt_t * p_pkt)
{
	assert(p_endpoint);
	assert(p_pkt);
	
	queue_init(&p_pkt->node);
	queue_add_tail(&p_pkt->node, &p_endpoint->m_send_queue);
	p_endpoint->m_send_queue_pkt_num++;	
	
	return 0;
}

int coap_recv_queue_push_node(coap_endpoint_t * p_endpoint, coap_pkt_t * p_pkt)
{
	assert(p_endpoint);
	assert(p_pkt);
	
	queue_init(&p_pkt->node);
	queue_add_tail(&p_pkt->node, &p_endpoint->m_recv_queue);
	p_endpoint->m_send_queue_pkt_num++;	
	
	return 0;
}

int coap_send_queue_del_node(coap_endpoint_t * p_endpoint, uint16 message_id)
{
	coap_pkt_t * p_pkt = NULL;
	queue_hdr_t * p = NULL;
	queue_hdr_t * next = NULL;

	assert(p_endpoint);
	
	for (p = p_endpoint->m_send_queue.next; p != &p_endpoint->m_send_queue; p = next)
	{
		coap_pkt_t * p_pkt = queue_entry(p, coap_pkt_t, node);
		next = p->next;
		if (p_pkt->hdr.message_id == message_id) 
		{
			queue_del(&p_pkt->node);
			free(p_pkt);
			p_endpoint->m_send_queue_pkt_num--;		
			return 0;
		}
	}

	return -1;
}

int coap_recv_queue_pop_node(coap_endpoint_t * p_endpoint, char * buffer, size_t len)
{
	coap_pkt_t * p_pkt = NULL;

	assert(p_endpoint);
	assert(buffer);
	assert(len > 0);

	if (queue_is_empty(&p_endpoint->m_recv_queue))
	{
		return 0;
	}
	
	p_pkt = queue_entry(&p_endpoint->m_recv_queue, coap_pkt_t, node);
	
	if (p_pkt->len > len)
	{
		return -1;
	}
	
	memcpy(buffer, &p_pkt->hdr, p_pkt->len);
	queue_del(&p_pkt->node);
	free(p_pkt);
	p_endpoint->m_recv_queue_pkt_num--;
	
	return len;
}
