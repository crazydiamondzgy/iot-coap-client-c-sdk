#ifndef __COAP_ENDPOINT_H__
#define __COAP_ENDPOINT_H__

#include "mutex.h"
#include "queue.h"
#include "coap_type.h"
#include "coap_packet.h"
#include "os_support.h"

#define COAP_MAX_ENDPOINTS  16
#define COAP_MAX_RETRANSMIT 4
#define COAP_ACK_TIMEOUT    2000

typedef struct 
{
	struct sockaddr_in m_servaddr;
	socket_t           m_servsock;
	queue_hdr_t        m_send_queue;
	queue_hdr_t        m_recv_queue;
	uint32             m_send_queue_pkt_num;
	uint32             m_recv_queue_pkt_num;
	mutex_t            m_send_mutex;
	mutex_t            m_recv_mutex;
	uint16             m_message_id;
} coap_endpoint_t;

typedef struct 
{
	coap_endpoint_t * m_coap_endpoints[COAP_MAX_ENDPOINTS];
	pthread_t     m_endpoint_mgr_thread;
	uint32        m_endpoint_mgr_time_cache;
	int           m_endpoint_mgr_is_running;
} coap_endpoint_mgr_t;

coap_endpoint_t * coap_alloc_endpoint();
coap_endpoint_t * coap_get_endpoint(coap_endpoint_mgr_t * p_endpoint_mgr, int i);
int coap_find_unused_endpoint(coap_endpoint_mgr_t * p_endpoint_mgr);
int coap_clear_endpoints(coap_endpoint_mgr_t * p_endpoint_mgr);
int coap_init_endpoints(coap_endpoint_mgr_t * p_endpoint_mgr);
int coap_set_endpoint(coap_endpoint_mgr_t * p_endpoint_mgr, int i, coap_endpoint_t * p_endpoint);
int coap_free_endpoint(coap_endpoint_mgr_t * p_endpoint_mgr, int i);
int coap_send_queue_push_node(coap_endpoint_t * p_endpoint, coap_pkt_t * p_pkt);
int coap_send_queue_del_node(coap_endpoint_t * p_endpoint, uint16 message_id);
int coap_recv_queue_push_node(coap_endpoint_t * p_endpoint, coap_pkt_t * p_pkt);
int coap_recv_queue_pop_node(coap_endpoint_t * p_endpoint, char * buffer, size_t len);
uint16 coap_get_message_id(coap_endpoint_t * p_endpoint);

#endif
