#include <assert.h>
#include "coap_type.h"
#include "coap_opt.h"
#include "coap_log.h"

#define ADVANCE_OPT(o,e,step) if((e)<step){return 0;}else{(e)-=step;(o)=((uint8*)(o))+step;}

size_t coap_opt_parse(uint8 * opt, size_t length, coap_option_t * result)
{
	uint8 * opt_start = opt;
	
	assert(opt);
	assert(result);
		
	if (length < 1)
		return 0;
	
	result->delta = (*opt & 0xf0) >> 4;
	result->length = *opt & 0x0f;
	
	switch(result->delta) 
	{
	case 15:
		if (*opt != 0xFF) 
		{
			coap_log_error_string("ignored reserved option delta 15\n");
		}
		return 0;
	case 14:
    	ADVANCE_OPT(opt,length,1);
		result->delta = ((*opt & 0xff) << 8) + 269;
		if (result->delta < 269)
		{
			coap_log_error_string("delta too large\n");
			return 0;
		}
	case 13:
		ADVANCE_OPT(opt,length,1);
		result->delta += *opt & 0xff;
		break;
		
	default:
		break;
	}
	
	switch(result->length)
	{
	case 15:
		coap_log_error_string("found reserved option length 15\n");
		return 0;
	case 14:
    	ADVANCE_OPT(opt,length,1);
		result->length = ((*opt & 0xff) << 8) + 269;
	case 13:
		ADVANCE_OPT(opt,length,1);
		result->length += *opt & 0xff;
		break;
		
	default:
		;
	}
	
	ADVANCE_OPT(opt,length,1);
	
	result->value = (unsigned char *)opt;
	if (length < result->length) 
	{
		coap_log_error_string("invalid option length\n");
		return 0;
	}
		
	return (opt + result->length) - opt_start;
}

size_t coap_opt_is_valid(uint8 ** pp_opt, size_t * p_length) 
{
	coap_option_t option;
	size_t optsize;
	
	assert(pp_opt);
	assert(*pp_opt); 
	assert(p_length);
	
	optsize = coap_opt_parse(*pp_opt, *p_length, &option);
	if (optsize) 
	{
		assert(optsize <= *p_length);
		
		*pp_opt += optsize;
		*p_length -= optsize;
	}
	
	return optsize;
}
