#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>

#include "coap_pkt.h"
#include "coap_log.h"
#include "coap_opt.h"

coap_pkt_t * coap_pkt_alloc(size_t len)
{
	size_t size = sizeof(coap_pkt_t) - sizeof(coap_hdr_t) + len;
	coap_pkt_t * p_pkt = (coap_pkt_t *)malloc(size + 1);
	if (p_pkt) 
	{
		memset(p_pkt, 0, size + 1);
		p_pkt->size = size;
	}

	return p_pkt;
}

int coap_pkt_free(coap_pkt_t * p_pkt) 
{
	if (p_pkt)
	{
		free(p_pkt);
		return 0;
	}

	return -1;
}

int coap_pkt_add_header(coap_pkt_t * p_pkt, uint8 type, uint8 code, uint16 message_id) 
{
	p_pkt->packet_length  = 4;
	p_pkt->hdr.version    = 1;
	p_pkt->hdr.type       = type;
	p_pkt->hdr.code       = code;
	p_pkt->hdr.message_id = message_id;
	
	return 0;
}

int coap_pkt_add_token(coap_pkt_t * p_pkt, uint8 * token_data, size_t token_len) 
{
	size_t pkt_len = token_len + p_pkt->packet_length;
	if (!p_pkt || token_len > 8 || p_pkt->size < pkt_len)
	{
		return -1;
	}
	
	if (token_len)
	{
		memcpy(p_pkt->hdr.token, token_data, token_len);
	}
	
	p_pkt->hdr.token_length = token_len;
	p_pkt->packet_length    = pkt_len;
	p_pkt->max_delta        = 0;
	
	return 0;
}

int coap_pkt_add_option(coap_pkt_t * p_pkt, uint16 option_type, char * option_data, size_t option_len)
{
	uint8 * p_opt = (uint8 *)&p_pkt->hdr + p_pkt->packet_length;
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

	if (NULL ==  p_pkt->p_option)
	{
		p_pkt->p_option  = p_opt;
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
	p_pkt->packet_length += option_len + i + 1;

	return 0;
}

int coap_pkt_add_data(coap_pkt_t * p_pkt, uint8 * p_data, size_t data_len) 
{
	uint8 * p = (uint8 *)&p_pkt->hdr + p_pkt->packet_length;
	if (data_len == 0)
	{
		coap_log_error_string("there is no data in input buffer\r\n");
		return -1;
	}
	
	if (p_pkt->packet_length + data_len + 1 > p_pkt->size)
	{
		coap_log_error_string("insufficient space to store input data\r\n");
		return -2;
	}
	
	*p++ = 0xFF;
	memcpy(p, p_data, data_len);
	p_pkt->p_data = p;
	p_pkt->packet_length += data_len + 1;
	
	return 0;
}

coap_pkt_t * coap_pkt_parse(uint8 * p_data, size_t len) 
{
	uint8 * p_option = NULL;
	coap_pkt_t * p_pkt = NULL;

	assert(p_data);

	if (COAP_MAX_PKT_SIZE < len) 
	{
		coap_log_error_string("insufficient space to store parsed packet\r\n");
		return NULL;
	}

	if (len < sizeof(coap_hdr_t) - 1)
	{
		coap_log_error_string("input buffer is too small to be a coap packet\r\n");
		return NULL;
	}

	p_pkt = coap_pkt_alloc(len);
	p_pkt->packet_length = len;
	memcpy(&p_pkt->hdr, p_data, len);

	if (0 == p_pkt->hdr.code) 
	{
		if (len != sizeof(coap_hdr_t) - 1 || p_pkt->hdr.token_length)
		{
			coap_log_error_string("empty message is not empty\r\n");
			goto discard;
		}
	}

	if (len < sizeof(coap_hdr_t) - 1 + p_pkt->hdr.token_length || p_pkt->hdr.token_length > 8)
	{
		coap_log_error_string("invalid token length\r\n");
		goto discard;
	}

	len -= p_pkt->hdr.token_length + 4;
	p_pkt->p_option = p_option = (uint8 *)&p_pkt->hdr + p_pkt->hdr.token_length + 4;
	
	while (len && *p_option != 0xFF)
	{
		if (!coap_opt_is_valid(&p_option, (size_t *)&len))
		{
			coap_log_error_string("coap option is not valid\r\n");
			goto discard;
		}
	}

	p_pkt->option_length = p_option - p_pkt->p_option;

	if (len)
	{
		if (*p_option != 0xFF)
		{
			coap_log_error_string("coap payload without start marker\r\n");
			goto discard;
		}

		len--;
		p_option++;
		
		if (0 == len)
		{
			coap_log_error_string("coap with start marker without payload\r\n");
			goto discard;
		}

		p_pkt->data_length = len;
		p_pkt->p_data = (uint8 *)p_option;
	}

	return p_pkt;

discard:
	free(p_pkt);
	return NULL;
}
