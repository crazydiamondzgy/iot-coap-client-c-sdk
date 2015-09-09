#ifndef __COAP_TYPES_H__
#define __COAP_TYPES_H__

typedef unsigned char byte;
typedef unsigned char uint8;
typedef signed char int8;
typedef unsigned short uint16;
typedef signed short int16;
typedef unsigned int uint;
typedef unsigned int uint32;
typedef signed int int32;

#if defined _WIN32
#  if (_MSC_VER > 1200)
typedef unsigned __int64 uint64;
typedef signed __int64 int64;
#  else
typedef __int64 uint64;
typedef __int64 int64;
#  endif
#else
typedef unsigned long long uint64;
typedef long long int64;
#endif

#ifndef INT64_MAX
#define INT64_MAX 0x7fffffffffffffffLL
#endif

typedef const char * cstr;
typedef char * str;

#ifndef __cplusplus
typedef uint8 bool;
#define true 1
#define false 0
#endif

#endif
