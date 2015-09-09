#ifndef __OS_SUPPORT_H__
#define __OS_SUPPORT_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef _WIN32

#  include <winsock2.h>
#  include <ws2tcpip.h>
#  include <io.h>

#  define snprintf _snprintf
#  define vsnprintf _vsnprintf
#  ifndef strncasecmp
#    if defined(_MSC_VER) && (_MSC_VER >= 1400)
#      define strncasecmp _memicmp
#    else
#      define strncasecmp memicmp
#    endif
#  endif

typedef int              socklen_t;
typedef HANDLE           pthread_t;
typedef SOCKET           socket_t;

#else

#  include <ctype.h>
#  include <unistd.h>
#  include <sys/select.h>
#  include <sys/socket.h>
#  include <sys/types.h>
#  include <sys/param.h>
#  include <sys/time.h>
#  include <netinet/in.h>
#  include <arpa/inet.h>
#  include <netdb.h>
#  include <net/if.h>
#  include <poll.h>
#  include <strings.h>
#  include <errno.h>
#  define closesocket close
#  define min(a,b)	((a) < (b) ? (a) : (b))
#  define max(a,b)	((a) > (b) ? (a) : (b))

typedef int              socket_t;

#endif

#endif
