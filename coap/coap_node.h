#ifndef __COAP_NODE_H__
#define __COAP_NODE_H__

#ifdef __cplusplus
extern "C" {
#endif

#include "utypes.h"

#define COAP_MAX_NODES  32

typedef struct 
{
	sockaddr_in m_servaddr;
	SOCKET      m_servsock;
} coap_node_t;

typedef struct 
{
	coap_node_t * m_coap_nodes[COAP_MAX_NODES];
	pthread_t     m_node_mgr_thread;
	uint32        m_node_mgr_time_cache;
} coap_node_mgr_t;

int coap_find_unused_endpoint(coap_node_mgr_t * p_node_mgr);
int coap_clear_nodes(coap_node_mgr_t * p_node_mgr);
int coap_set_node(coap_node_mgr_t * p_node_mgr, int i, coap_node_t * p_node);
coap_node_t * coap_get_node(coap_node_mgr_t * p_node_mgr, int i);

#ifdef __cplusplus
}
#endif

#endif
