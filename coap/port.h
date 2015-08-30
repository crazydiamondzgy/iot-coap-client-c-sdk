#ifndef __PORT_H__
#define __PORT_H__

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef WIN32

#  include <winsock2.h>
#  include <ws2tcpip.h>
#  include <io.h>

#  define snprintf _snprintf
#  ifndef strncasecmp
#    if defined(_MSC_VER) && (_MSC_VER >= 1400)
#      define strncasecmp _memicmp
#    else /* defined(_MSC_VER) && (_MSC_VER >= 1400) */
#      define strncasecmp memicmp
#    endif /* defined(_MSC_VER) && (_MSC_VER >= 1400) */
#  endif /* #ifndef strncasecmp */

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

#endif
