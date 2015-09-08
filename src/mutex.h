#ifndef __MUTEX_H__
#define __MUTEX_H__

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
} mutex_t;

void mutex_init(mutex_t * p_mutex);
void mutex_uninit(mutex_t * p_mutex);
void mutex_lock(mutex_t * p_mutex);
void mutex_unlock(mutex_t * p_mutex);

#endif
