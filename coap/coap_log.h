#ifndef __COAP_LOG_H__
#define __COAP_LOG_H__

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#pragma warning(disable:4127)
#else
#include <sys/time.h>
#endif

#include "utypes.h"

#define COAP_LOG_MAX_BUFSIZE   4096
#define COAP_LOG_LEVEL_DEBUG   0 
#define COAP_LOG_LEVEL_INFO    1 
#define COAP_LOG_LEVEL_WARN    2 
#define COAP_LOG_LEVEL_ERROR   3 
		
void coap_log_print_string(int level, const char * file_name, int line_number, char * format, ...);
void coap_log_print_binary(int level, const char * file_name, int line_number, char * buf, int buflen);

#ifdef _WIN32
#  define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)
#else
#  define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#endif

#define coap_log_debug_string(format, ...) coap_log_print_string(COAP_LOG_LEVEL_DEBUG, __FILENAME__, __LINE__, (char *)format, ##__VA_ARGS__)
#define coap_log_debug_binary(buf, buflen) coap_log_print_binary(COAP_LOG_LEVEL_DEBUG, __FILENAME__, __LINE__, (char *)buf, buflen)
#define coap_log_info_string(format, ...)  coap_log_print_string(COAP_LOG_LEVEL_INFO,  __FILENAME__, __LINE__, (char *)format, ##__VA_ARGS__)
#define coap_log_warn_string(format, ...)  coap_log_print_string(COAP_LOG_LEVEL_WARN,  __FILENAME__, __LINE__, (char *)format, ##__VA_ARGS__)
#define coap_log_error_string(format, ...) coap_log_print_string(COAP_LOG_LEVEL_ERROR, __FILENAME__, __LINE__, (char *)format, ##__VA_ARGS__)

#endif
