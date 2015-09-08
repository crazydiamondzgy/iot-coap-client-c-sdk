#include <stdio.h>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#else
#include <sys/time.h>
#endif

#ifdef __ANDROID__
#include <android/log.h>
#endif

#include "coap_log.h"
#include "coap_packet.h"
#include "os_support.h"

static char * __coap_log_level_string[COAP_LOG_LEVEL_MAX] =
{
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR"
};

static int __coap_log_level = COAP_LOG_LEVEL_DEBUG;

void coap_log_set_level(int level)
{
    __coap_log_level = level;
}

void coap_log_print(char * tmp, int level)
{
#ifdef _WIN32
	OutputDebugString(tmp);
	printf(tmp);
#elif __ANDROID__
	int android_log_level = ANDROID_LOG_UNKNOWN;
	switch (level)
	{
	case LEVEL_DEBUG:
		android_log_level = ANDROID_LOG_DEBUG;
		break;
	case LEVEL_INFO:
		android_log_level = ANDROID_LOG_INFO;
		break;
	case LEVEL_WARN:
		android_log_level = ANDROID_LOG_WARN;
		break;
	case LEVEL_ERROR:
		android_log_level = ANDROID_LOG_ERROR;
		break;
	default:
		break;
	}
	__android_log_print(android_log_level, "coap", tmp);
#else
	printf(tmp);
#endif
}

void coap_log_print_header(int level, const char * file_name, int line_number)
{
	char tmp[COAP_LOG_MAX_BUFSIZE];

#ifdef _WIN32
	SYSTEMTIME    sysTime;
	GetLocalTime(&sysTime);
	snprintf(tmp, sizeof(tmp), "%04d%02d%02d-%02d:%02d:%02d:%03d [%5s] %s: %04d: " ,
		sysTime.wYear, sysTime.wMonth, sysTime.wDay,
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
		sysTime.wMilliseconds, __coap_log_level_string[level], 
		file_name, line_number);
#elif __ANDROID__
#else
	struct tm *nowtime;
	time_t long_time;
	struct timeval t;
	gettimeofday(&t, NULL);
	time( &long_time );
	nowtime = localtime( &long_time );
	snprintf(tmp, sizeof(tmp), "%04d%02d%02d-%02d:%02d:%02d:%03d [%5s] %s: %04d: ",
		nowtime->tm_year + 1900, nowtime->tm_mon + 1, nowtime->tm_mday,
		nowtime->tm_hour, nowtime->tm_min, nowtime->tm_sec,
		(int)(t.tv_usec/1000), __coap_log_level_string[level], 
		file_name, line_number);
#endif

	coap_log_print(tmp, level);
}

void coap_log_debug_string(char* format, ...)
{
	va_list argp;
	char tmp[COAP_LOG_MAX_BUFSIZE];
	memset(tmp, 0, sizeof(tmp));

    if (NULL == format)
    {
        return;
    }

    if (COAP_LOG_LEVEL_DEBUG < __coap_log_level)
    {
        return;
    }
	
	coap_log_print_header(COAP_LOG_LEVEL_DEBUG, __FILENAME__, __LINE__);
	
	va_start(argp, format);
	vsnprintf(tmp, COAP_LOG_MAX_BUFSIZE, format, argp);
	va_end(argp);

	coap_log_print(tmp, COAP_LOG_LEVEL_DEBUG);

	return;
}

void coap_log_info_string(char* format, ...)
{
    va_list argp;
	char tmp[COAP_LOG_MAX_BUFSIZE];
	memset(tmp, 0, sizeof(tmp));

    if (NULL == format)
    {
        return;
    }

    if (COAP_LOG_LEVEL_INFO < __coap_log_level)
    {
        return;
    }

	coap_log_print_header(COAP_LOG_LEVEL_INFO, __FILENAME__, __LINE__);

    va_start(argp, format);
	vsnprintf(tmp, COAP_LOG_MAX_BUFSIZE, format, argp);
    va_end(argp);

	coap_log_print(tmp, COAP_LOG_LEVEL_INFO);

	return;
}

void coap_log_warn_string(char* format, ...)
{
    va_list argp;
	char tmp[COAP_LOG_MAX_BUFSIZE];
	memset(tmp, 0, sizeof(tmp));

    if (NULL == format)
    {
        return;
    }

    if (COAP_LOG_LEVEL_WARN < __coap_log_level)
    {
        return;
    }

	coap_log_print_header(COAP_LOG_LEVEL_WARN, __FILENAME__, __LINE__);

    va_start(argp, format);
	vsnprintf(tmp, COAP_LOG_MAX_BUFSIZE, format, argp);
    va_end(argp);

	coap_log_print(tmp, COAP_LOG_LEVEL_WARN);

	return;
}

void coap_log_error_string(char* format, ...)
{
    va_list argp;
	char tmp[COAP_LOG_MAX_BUFSIZE];
	memset(tmp, 0, sizeof(tmp));

    if (NULL == format)
    {
        return;
    }

    if (COAP_LOG_LEVEL_ERROR < __coap_log_level)
    {
        return;
    }

	coap_log_print_header(COAP_LOG_LEVEL_ERROR, __FILENAME__, __LINE__);

    va_start(argp, format);
	vsnprintf(tmp, COAP_LOG_MAX_BUFSIZE, format, argp);
    va_end(argp);

	coap_log_print(tmp, COAP_LOG_LEVEL_ERROR);

	return;
}

void coap_log_debug_binary(char * buf, int buflen)
{
	char tmp[COAP_LOG_MAX_BUFSIZE];
	memset(tmp, 0, sizeof(tmp));

	if (COAP_LOG_LEVEL_DEBUG < __coap_log_level)
	{
		return;
	}

	coap_log_print_header(COAP_LOG_LEVEL_DEBUG, __FILENAME__, __LINE__);

	while (buflen--)
	{
		snprintf(tmp, COAP_LOG_MAX_BUFSIZE, "%s%02X%s", tmp, 0xFF & (*buf++), (buflen > 0) ? " " : "\r\n");
	}

	coap_log_print(tmp, COAP_LOG_LEVEL_DEBUG);
	
	return;
}

void coap_log_debug_packet(char * buf)
{
	uint8 i = 0;
	uint8 code_class = 0;
	uint8 code_detail = 0;
	uint16 message_id = 0;
	char * type;
	char tmp[COAP_LOG_MAX_BUFSIZE];
	coap_pkt_t * p_pkt = (coap_pkt_t *)buf;
	memset(tmp, 0, sizeof(tmp));

	if (COAP_LOG_LEVEL_DEBUG < __coap_log_level)
	{
		return;
	}

	switch (p_pkt->hdr.type)
	{
	case 0: type = "CON"; break;
	case 1: type = "NON"; break;
	case 2: type = "ACK"; break;
	case 3: type = "RST"; break;
	default: return;
	}

	code_class = p_pkt->hdr.code >> 5;
	code_detail = p_pkt->hdr.code & 0x1F;
	message_id = ntohs(p_pkt->hdr.message_id);
	coap_log_print_header(COAP_LOG_LEVEL_DEBUG, __FILENAME__, __LINE__);

	snprintf(tmp, COAP_LOG_MAX_BUFSIZE, "ver=%d, type=%s, tkl=%d, code=%X.%02X, id=%04X {", 
		p_pkt->hdr.version, type, p_pkt->hdr.token_length, code_class, code_detail, message_id);
	
	for (i = 0; i < p_pkt->hdr.token_length; i++) 
	{
		snprintf(tmp, COAP_LOG_MAX_BUFSIZE, "%s%02x", tmp, p_pkt->hdr.token[i]);
	}
  
	snprintf(tmp, COAP_LOG_MAX_BUFSIZE, "%s}", tmp);

	if (p_pkt->p_data)
	{
		snprintf(tmp, COAP_LOG_MAX_BUFSIZE, "%s::%s\r\n", tmp, p_pkt->p_data);
	}

	coap_log_print(tmp, COAP_LOG_LEVEL_DEBUG);
	
	return;
}