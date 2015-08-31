#ifndef __COAP_MUTEX_H__
#define __COAP_MUTEX_H__

#ifndef WINDOWS
#if defined(_WIN32) || defined(_WIN64)
#define WINDOWS
#endif
#endif

#ifndef WINDOWS
#include <pthread.h>
#else
#include <windows.h>
#endif

typedef struct
{
#ifdef WINDOWS
	HANDLE m_mutex;
#else
	pthread_mutex_t m_mutex;
#endif
	int m_is_created;
} coap_mutex_t;

coap_mutex_t * coap_mutex_alloc();
void coap_mutex_free(coap_mutex_t * p_mutex);
void coap_mutex_lock(coap_mutex_t * p_mutex);
void coap_mutex_unlock(coap_mutext_t * p_mutex);

#endif
