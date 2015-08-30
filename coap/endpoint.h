#ifndef __ENDPOINT_H__
#define __ENDPOINT_H__

#ifdef __cplusplus
extern "C" {
#endif

#define COAP_MAX_ENDPOINTS  32

typedef struct {
	sockaddr_in m_servaddr;
	SOCKET      m_servsock;
} coap_endpoint_t;

int coap_find_unused_endpoint();
int coap_clear_endpoints();
coap_endpoint_t * coap_get_endpoint(int i);
int coap_set_endpoint(int i, coap_endpoint_t * p_endpoint);

#ifdef __cplusplus
}
#endif

#endif
