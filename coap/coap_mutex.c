#include "p2p_mutex.h"

p2p_mutex_t::p2p_mutex_t() : m_bCreated(true)
{
#ifdef WINDOWS
	m_mutex = CreateMutex(NULL, FALSE, NULL);
	if (!m_mutex)
	{
		m_bCreated = false;
	}

#else
	pthread_mutexattr_t mattr;
	pthread_mutexattr_init( &mattr );
	pthread_mutex_init(&m_mutex,&mattr);

#endif
}

p2p_mutex_t::~p2p_mutex_t()
{
#ifdef WINDOWS
	WaitForSingleObject(m_mutex,INFINITE);
	CloseHandle(m_mutex);
#else
	pthread_mutex_lock(&m_mutex);
	pthread_mutex_unlock(&m_mutex);
	pthread_mutex_destroy(&m_mutex);
#endif
}

void p2p_mutex_t::Lock()
{
#ifdef WINDOWS
	WaitForSingleObject(m_mutex,INFINITE);
#else
	pthread_mutex_lock(&m_mutex);
#endif
}

void p2p_mutex_t::Unlock()
{
#ifdef WINDOWS
	ReleaseMutex(m_mutex);
#else
	pthread_mutex_unlock(&m_mutex);
#endif
}
