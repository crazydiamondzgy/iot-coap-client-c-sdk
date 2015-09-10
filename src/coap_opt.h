#ifndef __COAP_OPT_H__
#define __COAP_OPT_H__

#include "os_support.h"
#include "coap_type.h"

typedef struct 
{
  uint16  delta;
  size_t  length;
  uint8 * value;
} coap_option_t;

size_t coap_opt_is_valid(uint8 ** pp_opt, size_t * p_length);
size_t coap_opt_parse(uint8 * opt, size_t length, coap_option_t * result);

#endif
