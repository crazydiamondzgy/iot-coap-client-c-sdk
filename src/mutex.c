#include "mutex.h"

void mutex_init(mutex_t * p_mutex)
{
#ifdef WINDOWS
	p_mutex->m_mutex = CreateMutex(NULL, FALSE, NULL);
#else
	pthread_mutexattr_t mattr;
	pthread_mutexattr_init(&mattr);
	pthread_mutex_init(&p_mutex->m_mutex, &mattr);
#endif
}

void mutex_uninit(mutex_t * p_mutex)
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

void mutex_lock(mutex_t * p_mutex)
{
#ifdef WINDOWS
	WaitForSingleObject(p_mutex->m_mutex,INFINITE);
#else
	pthread_mutex_lock(&p_mutex->m_mutex);
#endif
}

void mutex_unlock(mutex_t * p_mutex)
{
#ifdef WINDOWS
	ReleaseMutex(p_mutex->m_mutex);
#else
	pthread_mutex_unlock(&p_mutex->m_mutex);
#endif
}
