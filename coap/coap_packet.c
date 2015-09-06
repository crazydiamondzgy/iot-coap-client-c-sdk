#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "coap_packet.h"

int coap_pkt_is_valid(coap_pkt_t * p_pkt)
{
	return 1;
}

coap_pkt_t * coap_pkt_alloc(size_t len)
{
	coap_pkt_t * p_pkt = malloc(sizeof(coap_pkt_t) - sizeof(coap_hdr_t) + len);
	if (p_pkt) 
	{
		memset(p_pkt, 0, sizeof(coap_pkt_t) - sizeof(coap_hdr_t) + len);
		p_pkt->size = COAP_MAX_PKT_SIZE;
	}

	return p_pkt;
}

int coap_pkt_free(coap_pkt_t * p_pkt) 
{
	free(p_pkt);

	return 0;
}

int coap_pkt_add_header(coap_pkt_t * p_pkt, uint8 type, uint8 code, uint16 message_id) 
{
	p_pkt->len            = 4;
	p_pkt->hdr.version    = 1;
	p_pkt->hdr.type       = type;
	p_pkt->hdr.code       = code;
	p_pkt->hdr.message_id = message_id;
	
	return 0;
}

int coap_pkt_add_token(coap_pkt_t * p_pkt, uint8 * token_data, size_t token_len) 
{
	size_t pkt_len = token_len + p_pkt->len;
	if (!p_pkt || token_len > 8 || p_pkt->size < pkt_len)
	{
		return -1;
	}
	
	if (token_len)
	{
		memcpy(p_pkt->hdr.token_data, token_data, token_len);
	}
	
	p_pkt->hdr.token_len = token_len;
	p_pkt->len = pkt_len;
	p_pkt->max_delta = 0;
	
	return 0;
}

int coap_pkt_add_option(coap_pkt_t * p_pkt, uint16 option_type, uint8 * option_data, size_t option_len) {
	uint8 * p_opt = (uint8 *)&p_pkt->hdr + p_pkt->len;
	uint16 option_delta = option_type - p_pkt->max_delta;
	int i = 0;
	
	if (0 == option_len)
	{
		return -1;
	}

	if (option_type < p_pkt->max_delta)
	{
		return -2;
	}
	
	if (option_delta < 13)
	{
		p_opt[0] = option_delta << 4;
	}
	else if (option_delta < 270) 
	{
		p_opt[0] = 0xD0;
		p_opt[++i] = option_delta - 13;
	} 
	else 
	{
		p_opt[0] = 0xE0;
		p_opt[++i] = ((option_delta - 269) >> 8) & 0xFF;
		p_opt[++i] = (option_delta - 269) & 0xFF;
	}
    
	if (option_len < 13)
	{
		p_opt[0] |= option_len & 0x0f;
	}
	else if (option_len < 270) 
	{
		p_opt[0] |= 0x0D;
		p_opt[++i] = option_len - 13;
	}
	else 
	{
		p_opt[0] |= 0x0E;
		p_opt[++i] = ((option_len - 269) >> 8) & 0xFF;
		p_opt[++i] = (option_len - 269) & 0xFF;
	}
	
	memcpy(p_opt + i + 1, option_data, option_len);
	
	p_pkt->max_delta = option_type;
	p_pkt->len += option_len + i + 1;

	return 0;
}

int coap_pkt_add_data(coap_pkt_t * p_pkt, uint8 * p_data, size_t data_len) 
{
	uint8 * p = (uint8 *)&p_pkt->hdr + p_pkt->len;
	if (data_len == 0)
	{
		return -1;
	}
	
	if (p_pkt->len + data_len + 1 > p_pkt->size)
	{
		return -2;
	}
	
	*p++ = 0xFF;
	memcpy(p, p_data, data_len);
	p_pkt->len += data_len + 1;
	
	return 0;
}
