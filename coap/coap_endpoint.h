#ifndef __COAP_ENDPOINT_H__
#define __COAP_ENDPOINT_H__

#include "coap_type.h"
#include "os_support.h"
#include "queue.h"

#define COAP_MAX_ENDPOINTS  32

typedef struct 
{
	struct sockaddr_in m_servaddr;
	socket_t           m_servsock;
} coap_endpoint_t;

typedef struct 
{
	coap_endpoint_t * m_coap_endpoints[COAP_MAX_ENDPOINTS];
	pthread_t     m_endpoint_mgr_thread;
	uint32        m_endpoint_mgr_time_cache;
	int           m_endpoint_mgr_is_running;
	queue_hdr_t   m_send_queue;
	queue_hdr_t   m_recv_queue;
} coap_endpoint_mgr_t;

int coap_find_unused_endpoint(coap_endpoint_mgr_t * p_endpoint_mgr);
int coap_clear_endpoints(coap_endpoint_mgr_t * p_endpoint_mgr);
int coap_init_endpoints(coap_endpoint_mgr_t * p_endpoint_mgr);
int coap_set_endpoint(coap_endpoint_mgr_t * p_endpoint_mgr, int i, coap_endpoint_t * p_endpoint);
coap_endpoint_t * coap_get_endpoint(coap_endpoint_mgr_t * p_endpoint_mgr, int i);

#endif
