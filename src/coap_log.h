#ifndef __COAP_LOG_H__
#define __COAP_LOG_H__

#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <string.h>

#include "coap_type.h"

#define COAP_LOG_MAX_BUFSIZE   4096
#define COAP_LOG_LEVEL_DEBUG   0 
#define COAP_LOG_LEVEL_INFO    1 
#define COAP_LOG_LEVEL_WARN    2 
#define COAP_LOG_LEVEL_ERROR   3 
#define COAP_LOG_LEVEL_MAX     4

#ifdef _WIN32
#  define __FILENAME__ (strrchr(__FILE__, '\\') ? (strrchr(__FILE__, '\\') + 1):__FILE__)
#else
#  define __FILENAME__ (strrchr(__FILE__, '/') ? (strrchr(__FILE__, '/') + 1):__FILE__)
#endif

void coap_log_set_level(int level);
void coap_log_debug_string(char * format, ...);
void coap_log_info_string(char * format, ...);
void coap_log_warn_string(char * format, ...);
void coap_log_error_string(char * format, ...);
void coap_log_debug_binary(char * buf, int buflen);
void coap_log_debug_packet(char * buf);

#endif
