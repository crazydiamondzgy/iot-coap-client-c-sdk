#include "coap_util.h"
#include "os_support.h"

#ifdef _WIN32

int coap_get_error_code()
{
	return GetLastError();
}

typedef ULONGLONG (WINAPI GetTickCount64Proc)(void);
static GetTickCount64Proc *pt2GetTickCount64;
static GetTickCount64Proc *pt2RealGetTickCount;

static uint64 startPerformanceCounter;
static uint64 startGetTickCount;
// MSVC 6 standard doesn't like division with uint64s
static double counterPerMicrosecond;

uint64 UTGetTickCount64()
{
	if (pt2GetTickCount64) {
		return pt2GetTickCount64();
	}
	if (pt2RealGetTickCount) {
		uint64 v = pt2RealGetTickCount();
		// fix return value from GetTickCount
		return (DWORD)v | ((v >> 0x18) & 0xFFFFFFFF00000000);
	}
	return (uint64)GetTickCount();
}

void Time_Initialize()
{
	uint64 frequency;

	HMODULE kernel32 = GetModuleHandleA("kernel32.dll");
	pt2GetTickCount64 = (GetTickCount64Proc*)GetProcAddress(kernel32, "GetTickCount64");
	pt2RealGetTickCount = (GetTickCount64Proc*)GetProcAddress(kernel32, "GetTickCount");
	QueryPerformanceCounter((LARGE_INTEGER*)&startPerformanceCounter);
	QueryPerformanceFrequency((LARGE_INTEGER*)&frequency);
	counterPerMicrosecond = (double)frequency / 1000000.0f;
	startGetTickCount = UTGetTickCount64();
}

int64 abs64(int64 x) { return x < 0 ? -x : x; }

static uint64 GetMicroseconds()
{
	uint64 counter;
	uint64 tick;
	int64  ret;
	int64 tick_diff;
	static bool time_init = false;
	if (!time_init) {
		time_init = true;
		Time_Initialize();
	}

	QueryPerformanceCounter((LARGE_INTEGER*) &counter);
	tick = UTGetTickCount64();

	// unfortunately, QueryPerformanceCounter is not guaranteed
	// to be monotonic. Make it so.
	ret = (int64)(((int64)counter - (int64)startPerformanceCounter) / counterPerMicrosecond);
	// if the QPC clock leaps more than one second off GetTickCount64()
	// something is seriously fishy. Adjust QPC to stay monotonic
	tick_diff = tick - startGetTickCount;
	if (abs64(ret / 100000 - tick_diff / 100) > 10) {
		startPerformanceCounter -= (uint64)((int64)(tick_diff * 1000 - ret) * counterPerMicrosecond);
		ret = (int64)((counter - startPerformanceCounter) / counterPerMicrosecond);
	}
	return ret;
}

#else //!WIN32

#include <time.h>
#include <sys/time.h>		// Linux needs both time.h and sys/time.h
#include <stdlib.h>

#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#if defined(__APPLE__)
#include <mach/mach_time.h>

int coap_get_error_code()
{
	return errno;
}

static uint64 GetMicroseconds()
{
	// http://developer.apple.com/mac/library/qa/qa2004/qa1398.html
	// http://www.macresearch.org/tutorial_performance_and_time
	static mach_timebase_info_data_t sTimebaseInfo;
	static uint64_t start_tick = 0;
	uint64_t tick;
	// Returns a counter in some fraction of a nanoseconds
	tick = mach_absolute_time();  
	if (sTimebaseInfo.denom == 0) {
		// Get the timer ratio to convert mach_absolute_time to nanoseconds
		mach_timebase_info(&sTimebaseInfo); 
		start_tick = tick;
	}
	// Calculate the elapsed time, convert it to microseconds and return it.
	return ((tick - start_tick) * sTimebaseInfo.numer) / (sTimebaseInfo.denom * 1000);
}

#else //!__APPLE__

/* Unfortunately, #ifdef CLOCK_MONOTONIC is not enough to make sure that
   POSIX clocks work -- we could be running a recent libc with an ancient
   kernel (think OpenWRT). -- jch */

static uint64_t GetMicroseconds()
{
	static int have_posix_clocks = -1;
	int rc;

#if defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0 && defined(CLOCK_MONOTONIC)
	if (have_posix_clocks < 0) {
		struct timespec ts;
		rc = clock_gettime(CLOCK_MONOTONIC, &ts);
		if (rc < 0) {
			have_posix_clocks = 0;
		} else {
			have_posix_clocks = 1;
		}
	}

	if (have_posix_clocks) {
		struct timespec ts;
		rc = clock_gettime(CLOCK_MONOTONIC, &ts);
		return uint64(ts.tv_sec) * 1000000 + ts.tv_nsec / 1000;
	}
#endif
	{
		struct timeval tv;
		rc = gettimeofday(&tv, NULL);
		return uint64(tv.tv_sec) * 1000000 + tv.tv_usec;
	}
}
#endif //!__APPLE__

#endif //!WIN32

uint64 coap_get_microseconds()
{
	static uint64 offset = 0, previous = 0;

	uint64 now = GetMicroseconds() + offset;
	if (previous > now) {
		/* Eek! */
		offset += previous - now;
		now = previous;
	}
	previous = now;
	return now;
}

uint32 coap_get_milliseconds()
{
	return (uint32)(coap_get_microseconds() / 1000);
}
