#ifdef _WIN32
#include <winsock2.h>
#endif

#include "p2p_util.h"
#include "p2p_addr.h"
#include "p2p_log.h"
#ifdef __ANDROID__
#include <android/log.h>
#endif

#include <stdio.h>

CLog * CLog::m_pInstance = NULL;
const char CLog::LevelTypeString[LEVEL_MAX][8] =
{
	"DEBUG",
	"INFO",
	"WARN",
	"ERROR",
	"DUMP"
};

CLog::CLog()
{
	m_log_level = CLog::LEVEL_DEBUG;
	memset(m_log_file, 0, MAX_PATH);
}

CLog::~CLog()
{
	CLog::m_pInstance = NULL;
}

CLog * CLog::GetInstance()
{
	if (NULL == m_pInstance)
	{
		m_pInstance = new CLog;
	}

	return m_pInstance;
}

void CLog::SetLogLevel(LEVEL_TYPE level)
{
    m_log_level = level;
}

void CLog::SetLogFile(char * file_name)
{
	strncpy(m_log_file, file_name, MAX_PATH);;
}

void CLog::PrintLog(char * tmp, int level)
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
	__android_log_print(android_log_level, "P2P", tmp);
#else
	printf(tmp);
#endif

	if (strncmp(m_log_file, "", MAX_PATH) != 0)
	{
		long file_size = 0;
		FILE * log_fd = fopen(m_log_file, "a+");
		if (log_fd == NULL)
		{
			return;
		}

		if (fseek(log_fd, 0L, SEEK_END) == -1)
		{
			fclose(log_fd);
			return;
		}

		if ((file_size = ftell(log_fd)) == -1)
		{
			fclose(log_fd);
			return;
		}

		if (file_size >= 64*1024)
		{
			fclose(log_fd);
			if ((log_fd = fopen(m_log_file, "w")) == NULL)
			{
				return;
			}
		}

		fprintf(log_fd, "%s", tmp);
		fclose(log_fd);
	}
}

void CLog::PrintHeader(LEVEL_TYPE level, const char * file_name, int line_number)
{
	char tmp[LOG_BUF_LEN_MAX];
	memset(tmp, 0, sizeof(tmp));

#ifdef _WIN32
	SYSTEMTIME    sysTime;
	GetLocalTime(&sysTime);
	snprintf(tmp, sizeof(tmp), "%04d%02d%02d-%02d:%02d:%02d:%03d [%5s] %s: %04d: " ,
		sysTime.wYear, sysTime.wMonth, sysTime.wDay,
		sysTime.wHour, sysTime.wMinute, sysTime.wSecond,
		sysTime.wMilliseconds, LevelTypeString[level], 
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
		(int)(t.tv_usec/1000), LevelTypeString[level], 
		file_name, line_number);
#endif

	PrintLog(tmp, level);
}

void CLog::PrintString(LEVEL_TYPE level, const char * file_name, int line_number, char* format, ...)
{
	char tmp[LOG_BUF_LEN_MAX];
	memset(tmp, 0, sizeof(tmp));

    if (NULL == format)
    {
        return;
    }

    if (level < m_log_level)
    {
        return;
    }

	PrintHeader(level, file_name, line_number);

    va_list argp;
    va_start(argp, format);
	vsnprintf(tmp, LOG_BUF_LEN_MAX, format, argp);
    va_end(argp);

	PrintLog(tmp, level);

	return;
}

void CLog::PrintBinary(LEVEL_TYPE level, const char * file_name, int line_number, char * buf, int buflen)
{
	char tmp[LOG_BUF_LEN_MAX];
	memset(tmp, 0, sizeof(tmp));

	if (level < m_log_level)
	{
		return;
	}

	PrintHeader(level, file_name, line_number);

	while (buflen--)
	{
		snprintf(tmp, LOG_BUF_LEN_MAX, "%s%02X%s", tmp, 0xFF & (*buf++), (buflen > 0) ? " " : "\r\n");
	}

	PrintLog(tmp, level);
	
	return;
}

const char * CLog::GetPacketTypeString(int packet_type)
{
	const char * p = NULL;
	switch (packet_type)
	{
	case P2P_MSG_LOGIN_REQ:        p = "P2P_MSG_LOGIN_REQ"; break;
	case P2P_MSG_LOGIN_REP:        p = "P2P_MSG_LOGIN_REP"; break;
	case P2P_MSG_LOGOUT_REQ:       p = "P2P_MSG_LOGOUT_REQ"; break;
	case P2P_MSG_LOGOUT_REP:       p = "P2P_MSG_LOGOUT_REP"; break;
	case P2P_MSG_KEEP_ALIVE_REQ:   p = "P2P_MSG_KEEP_ALIVE_REQ"; break;
	case P2P_MSG_KEEP_ALIVE_REP:   p = "P2P_MSG_KEEP_ALIVE_REP"; break;
	case P2P_MSG_PUNCH_HOLE_REQ:   p = "P2P_MSG_PUNCH_HOLE_REQ"; break;
	case P2P_MSG_PUNCH_HOLE_REP:   p = "P2P_MSG_PUNCH_HOLE_REP"; break;
	case P2P_MSG_OPEN_SESSION_REQ: p = "P2P_MSG_OPEN_SESSION_REQ"; break;
	case P2P_MSG_OPEN_SESSION_REP: p = "P2P_MSG_OPEN_SESSION_REP"; break;
	case P2P_MSG_FREE_SESSION_REQ: p = "P2P_MSG_FREE_SESSION_REQ"; break;
	case P2P_MSG_FREE_SESSION_REP: p = "P2P_MSG_FREE_SESSION_REP"; break;
	case P2P_MSG_OPEN_CHANNEL_REQ: p = "P2P_MSG_OPEN_CHANNEL_REQ"; break;
	case P2P_MSG_OPEN_CHANNEL_REP: p = "P2P_MSG_OPEN_CHANNEL_REP"; break;
	case P2P_MSG_FREE_CHANEEL_REQ: p = "P2P_MSG_FREE_CHANEEL_REQ"; break;
	case P2P_MSG_FREE_CHANNEL_REP: p = "P2P_MSG_FREE_CHANNEL_REP"; break;
	case P2P_MSG_HOST_NOT_EXIST:   p = "P2P_MSG_HOST_NOT_EXIST"; break;
	default:                       p = "P2P_MSG_UNKNOWN"; break;
	}

	return p;
}

void CLog::PrintPacket(LEVEL_TYPE level, const char * file_name, int line_number, char * buf)
{
	char tmp[LOG_BUF_LEN_MAX];
	memset(tmp, 0, sizeof(tmp));
	up2p_pkt_t * p_pkt = (up2p_pkt_t *)buf;
	const char * p_msg_str;

	if (level < m_log_level)
	{
		return;
	}

	PrintHeader(level, file_name, line_number);

	p_msg_str = GetPacketTypeString(p_pkt->msg);

	snprintf(tmp, LOG_BUF_LEN_MAX, "%010u<-%010u, ver=%X, type=%X, msg=%X(%s), dat=%X, crc=%X\r\n", 
		p_pkt->dst, p_pkt->src, p_pkt->get_version(), p_pkt->get_type(), p_pkt->msg, p_msg_str, p_pkt->dat, p_pkt->crc);

	PrintLog(tmp, level);

	return;
}

void CLog::PrintPacketKeepAlive(LEVEL_TYPE level, const char * file_name, int line_number, char * buf)
{
	char tmp[LOG_BUF_LEN_MAX];
	char ntop[20];
	memset(tmp, 0, sizeof(tmp));
	memset(ntop, 0, sizeof(ntop));
	up2p_pkt_keep_alive_t * p_pkt = (up2p_pkt_keep_alive_t *)buf;
	const char * p_msg_str;

	if (level < m_log_level)
	{
		return;
	}

	PrintHeader(level, file_name, line_number);

	p_msg_str = GetPacketTypeString(p_pkt->hdr.msg);

	snprintf(tmp, LOG_BUF_LEN_MAX, "%010u<-%010u, ver=%X, type=%X, msg=%X(%s), dat=%X, crc=%X, lan=(%s:%d)\r\n", 
		p_pkt->hdr.dst, p_pkt->hdr.src, p_pkt->hdr.get_version(), p_pkt->hdr.get_type(), p_pkt->hdr.msg, p_msg_str, p_pkt->hdr.dat, p_pkt->hdr.crc, 
		inet_ntop(AF_INET, &p_pkt->lan.ip, ntop, 20), ntohs(p_pkt->lan.port));

	PrintLog(tmp, level);

	return;
}

void CLog::PrintPacketPunchHole(LEVEL_TYPE level, const char * file_name, int line_number, char * buf)
{
	char tmp[LOG_BUF_LEN_MAX];
	char lan_addr[20];
	char wan_addr[20];
	memset(tmp, 0, sizeof(tmp));
	memset(lan_addr, 0, sizeof(lan_addr));
	memset(wan_addr, 0, sizeof(wan_addr));
	up2p_pkt_punch_hole_t * p_pkt = (up2p_pkt_punch_hole_t *)buf;
	const char * p_msg_str;

	if (level < m_log_level)
	{
		return;
	}

	PrintHeader(level, file_name, line_number);

	p_msg_str = GetPacketTypeString(p_pkt->hdr.msg);

	snprintf(tmp, LOG_BUF_LEN_MAX, "%010u<-%010u, ver=%X, type=%X, msg=%X(%s), dat=%X, crc=%X, lan=(%s:%d), wan=(%s:%d)\r\n", 
		p_pkt->hdr.dst, p_pkt->hdr.src, p_pkt->hdr.get_version(), p_pkt->hdr.get_type(), p_pkt->hdr.msg, p_msg_str, p_pkt->hdr.dat, p_pkt->hdr.crc, 
		inet_ntop(AF_INET, &p_pkt->lan.ip, lan_addr, 20), ntohs(p_pkt->lan.port), inet_ntop(AF_INET, &p_pkt->wan.ip, wan_addr, 20), ntohs(p_pkt->wan.port));

	PrintLog(tmp, level);

	return;
}
