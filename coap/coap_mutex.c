#include "coap_mutex.h"

coap_mutex_t * coap_mutex_alloc()
{
	coap_mutex_t * p_mutex = (coap_mutex_t *)malloc(sizeof(coap_mutex_t));

#ifdef WINDOWS
	p_mutex->m_mutex = CreateMutex(NULL, FALSE, NULL);
#else
	pthread_mutexattr_t mattr;
	pthread_mutexattr_init(&mattr);
	pthread_mutex_init(&p_mutex->m_mutex, &mattr);
#endif

	return p_mutex;
}

void coap_mutex_free(coap_mutex_t * p_mutex)
{
#ifdef WINDOWS
	WaitForSingleObject(p_mutex->m_mutex, INFINITE);
	CloseHandle(p_mutex->m_mutex);
#else
	pthread_mutex_lock(&p_mutex->m_mutex);
	pthread_mutex_unlock(&p_mutex->m_mutex);
	pthread_mutex_destroy(&p_mutex->m_mutex);
#endif
}

void coap_mutex_lock(coap_mutex_t * p_mutex)
{
#ifdef WINDOWS
	WaitForSingleObject(p_mutex->m_mutex,INFINITE);
#else
	pthread_mutex_lock(&p_mutex->m_mutex);
#endif
}

void coap_mutex_unlock(coap_mutex_t * p_mutex)
{
#ifdef WINDOWS
	ReleaseMutex(p_mutex->m_mutex);
#else
	pthread_mutex_unlock(&p_mutex->m_mutex);
#endif
}
