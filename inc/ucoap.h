#ifndef __UCOAP_H__
#define __UCOAP_H__

#ifdef __cplusplus
extern "C" {
#endif

#define COAP_RET_INVALID_PARAMETERS              -1
#define COAP_RET_UNRESOLVED_HOST                 -2
#define COAP_RET_SOCKET_INIT_FAILED              -3
#define COAP_RET_NO_MORE_NODE                    -4
#define COAP_RET_BUFFER_TOO_SMALL                -5

/* Common API */

int coap_connect(char * p_str_addr, int port);
int coap_close(int s);
int coap_setsockopt(int s, int level, int optname, void * optval, int optlen);
int coap_getsockopt(int s, int level, int optname, void * optval);

/* Synchronous API */

int coap_send(int s, char * p_method, char * p_url, char * p_data, int len);
int coap_recv(int s, char * p_data, int len);

/* Asynchronous API */

// coap_on_write
// coap_on_read
// coap_check_timeouts
// coap_is_valid_packet
// coap_process_packet

#ifdef __cplusplus
}
#endif

#endif
