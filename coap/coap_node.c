#include "endpoints.h"

int coap_clear_nodes(coap_node_mgr_t * p_node_mgr)
{
	assert(p_node_mgr);

	memset(p_node_mgr->m_coap_nodes, 0, sizeof(coap_endpoints_t)*COAP_MAX_NODES);
}

int coap_init_nodes(coap_node_mgr_t * p_node_mgr)
{
	assert(p_node_mgr);

	coap_clear_nodes();
	
#ifndef _WIN32
	pthread_create(&p_node_mgr->m_node_mgr_thread, NULL, coap_work_thread, p_node_mgr);
#else
	p_node_mgr->m_node_mgr_thread = CreateThread(NULL, 0, coap_work_thread, p_node_mgr, 0, NULL);
#endif
}

int coap_find_unused_node(coap_node_mgr_t * p_node_mgr)
{
	assert(p_node_mgr);

	int i = 0;
	for (; i < COAP_MAX_NODES; i++)
	{
		if (p_node_mgr->m_coap_nodes[i])
		{
			return i;
		}
	}

	return -1;
}

coap_node_t * coap_get_node(coap_node_mgr_t * p_node_mgr, int i)
{
	assert(p_node_mgr);

	if (i < COAP_MAX_NODES && i > 0)
	{
		return p_node_mgr->m_coap_nodes[i];
	}

	return NULL;
}

int coap_set_endpoint(coap_node_mgr_t * p_node_mgr, int i, coap_node_t * p_endpoint)
{
	assert(p_node_mgr);

	if (i < COAP_MAX_NODES && i > 0)
	{
		assert(p_node_mgr->m_coap_nodes[i] == NULL);
		p_node_mgr->m_coap_nodes[i] = p_endpoint;
	}

	return -1;
}

#ifndef _WIN32
void * coap_work_thread(void * p_void)
#else
DWORD WINAPI coap_work_thread(LPVOID p_void)
#endif
{
	coap_node_mgr_t * p_node_mgr = (coap_node_mgr_t *)p_void;

	char buf[2048];
	sockaddr_in sock;
	int udp_sock_len = sizeof(sock);
	int ret = 0;
	fd_set rfds;

	while (true == m_is_session_manager_running) 
	{
		p_node_mgr->m_node_mgr_time_cache = coap_get_milliseconds();

		coap_check_timeouts();

		FD_ZERO(&rfds);
		FD_SET(m_msg_sock, &rfds);

		timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 10000;
		ret = select(m_msg_sock + 1, &rfds, NULL, NULL, &tv);
		if (ret < 0) {
			p2p_log_error_string("## socket error, please restart libp2p\r\n");

			break;
		}
		else if (0 == ret) {
			continue;
		}
		else if (FD_ISSET(m_msg_sock, &rfds)) {
			up2p_pkt_t * p_pkt = (up2p_pkt_t *)buf;
			memset(p_pkt, 0, 2048);
			memset(&sock, 0, sizeof(sock));

			int len = recvfrom(m_msg_sock, (char *)buf, 2048, 0, (sockaddr *)&sock, (socklen_t *)&udp_sock_len);
			if (len <= 0) {
				int err = p2p_get_error_code();
				p2p_log_error_string("Socket Error:%d!\r\n", err);
				continue;
			}

			if (coap_is_valid_packet(p_pkt)) 
			{
				
				continue;
			}

			p2p_log_debug_string("## Incorrect Packet, Neither UP2P Nor UDTP\r\n");
			p2p_log_debug_binary(buf, len);
		}
	}

	p2p_log_error_string("## libp2p main thread exited\r\n");
	return;
}
