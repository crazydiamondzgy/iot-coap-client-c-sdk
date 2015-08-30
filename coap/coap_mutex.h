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

class coap_mutex_t
{
private:
#ifdef WINDOWS
	HANDLE m_mutex;
#else
	pthread_mutex_t m_mutex;
#endif

public:
	bool m_bCreated;
	void Lock();
	void Unlock();
	p2p_mutex_t(void);
	~p2p_mutex_t(void);
};

#endif
