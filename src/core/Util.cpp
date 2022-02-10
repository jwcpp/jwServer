#include "Util.h"
#include "Platform.h"

#if defined(SYSTEM_WIN)
#include <Windows.h>
#include <direct.h>
#endif

#if (defined(WIN32) || defined(_WIN32) || defined(__WIN32__))
struct tm* localtime_r(time_t const* time, struct tm* result)
{
	localtime_s(result, time);
	return result;
}
#endif


namespace Util{
	static std::random_device g_rDevice;
	static std::mt19937 g_mtGen(g_rDevice());

	std::mt19937& getMtGen() {return g_mtGen;}
}


uint32 Util::getTid()
{
#if defined(SYSTEM_WIN)
	return GetCurrentThreadId();
#else
	return pthread_self();
#endif
}

uint32 Util::getPid()
{
#ifdef SYSTEM_WIN
	DWORD pid = GetCurrentProcessId();
#else
	pid_t pid = getpid();
#endif

	return uint32(pid);
}

void Util::localtime_s(struct tm* t, const time_t* time)
{

#ifdef SYSTEM_WIN
	::localtime_s(t, time);
#else
	::localtime_r(time, t);
#endif
}

void Util::gmtime_s(struct tm* t, const time_t* time)
{
#ifdef SYSTEM_WIN
	::gmtime_s(t, time);
#else
	::gmtime_r(time, t);
#endif
}

std::string Util::timeToTimestampStr(time_t t)
{
	tm aTm;
	localtime_r(&t, &aTm);
	//       YYYY   year
	//       MM     month (2 digits 01-12)
	//       DD     day (2 digits 01-31)
	//       HH     hour (2 digits 00-23)
	//       MM     minutes (2 digits 00-59)
	//       SS     seconds (2 digits 00-59)
	char tmp[24];
	sprintf_s(tmp,"%04d-%02d-%02d_%02d-%02d-%02d", aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec);
	return std::string(tmp);
}

int Util::charCount(char* pStr, char c)
{
	int nCount = 0;

	char* pTemp = pStr;
	while (*pTemp != '\0')
	{
		if (*pTemp == c)
		{
			nCount += 1;
		}

		pTemp += 1;
	}

	return nCount;
}

int Util::mkdir(const char* dir)
{
#ifdef SYSTEM_WIN
	return _mkdir(dir);
#else
	return ::mkdir(dir, 0755);
#endif
}