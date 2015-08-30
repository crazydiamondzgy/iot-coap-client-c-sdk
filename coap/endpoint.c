#include "endpoints.h"

static coap_endpoints_t __coap_endpoints[COAP_MAX_ENDPOINTS];

int coap_clear_endpoints()
{
	memset(__coap_endpoints, 0, sizeof(coap_endpoints_t)*COAP_MAX_ENDPOINTS);
}

int coap_find_unused_endpoint()
{
	int i = 0;
	for (; i < COAP_MAX_ENDPOINTS; i++)
	{
		if (__coap_endpoints[i])
		{
			return i;
		}
	}

	return -1;
}

coap_endpoint_t * coap_get_endpoint_by_index(int i)
{
	if (i < COAP_MAX_ENDPOINTS && i > 0)
	{
		return __coap_endpoints[i];
	}

	return NULL;
}

int coap_set_endpoint(int i, coap_endpoint_t * p_endpoint)
{
	if (i < COAP_MAX_ENDPOINTS && i > 0)
	{
		assert(__coap_endpoints[i] == NULL);
		__coap_endpoints[i] = p_endpoint;
	}

	return -1;
}
