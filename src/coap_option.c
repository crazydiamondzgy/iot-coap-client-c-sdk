#include "coap_type.h"
#include "coap_option.h"

size_t coap_opt_parse(const coap_opt_t *opt, size_t length, coap_option_t *result) 
{
	const coap_opt_t *opt_start = opt; /* store where parsing starts  */
	
	assert(opt);
	assert(result);
	
#define ADVANCE_OPT(o,e,step) if ((e) < step) {			\
    debug("cannot advance opt past end\n");			\
    return 0;							\
	} else {							\
    (e) -= step;						\
    (o) = ((unsigned char *)(o)) + step;			\
	}
	
	if (length < 1)
		return 0;
	
	result->delta = (*opt & 0xf0) >> 4;
	result->length = *opt & 0x0f;
	
	switch(result->delta) {
	case 15:
		if (*opt != COAP_PAYLOAD_START) {
			debug("ignored reserved option delta 15\n");
		}
		return 0;
	case 14:
    /* Handle two-byte value: First, the MSB + 269 is stored as delta value.
	* After that, the option pointer is advanced to the LSB which is handled
		* just like case delta == 13. */
		ADVANCE_OPT(opt,length,1);
		result->delta = ((*opt & 0xff) << 8) + 269;
		if (result->delta < 269) {
			debug("delta too large\n");
			return 0;
		}
		/* fall through */
	case 13:
		ADVANCE_OPT(opt,length,1);
		result->delta += *opt & 0xff;
		break;
		
	default:
		;
	}
	
	switch(result->length) {
	case 15:
		debug("found reserved option length 15\n");
		return 0;
	case 14:
    /* Handle two-byte value: First, the MSB + 269 is stored as delta value.
	* After that, the option pointer is advanced to the LSB which is handled
		* just like case delta == 13. */
		ADVANCE_OPT(opt,length,1);
		result->length = ((*opt & 0xff) << 8) + 269;
		/* fall through */
	case 13:
		ADVANCE_OPT(opt,length,1);
		result->length += *opt & 0xff;
		break;
		
	default:
		;
	}
	
	ADVANCE_OPT(opt,length,1);
	/* opt now points to value, if present */
	
	result->value = (unsigned char *)opt;
	if (length < result->length) {
		debug("invalid option length\n");
		return 0;
	}
	
#undef ADVANCE_OPT
	
	return (opt + result->length) - opt_start;
}

static size_t next_option_safe(coap_opt_t **optp, size_t *length) {
	coap_option_t option;
	size_t optsize;
	
	assert(optp); assert(*optp); 
	assert(length);
	
	optsize = coap_opt_parse(*optp, *length, &option);
	if (optsize) {
		assert(optsize <= *length);
		
		*optp += optsize;
		*length -= optsize;
	}
	
	return optsize;
}
