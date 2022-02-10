#include "ColorConsoleAppender.h"
#include "Platform.h"
#include "Record.h"

#ifdef SYSTEM_WIN
#include <windows.h>
#endif

#include <iostream>

std::string makeLogInfo(const Record& record)
{
	tm aTm;
	localtime_r(record.getTime(), &aTm);
	char tmp[120];
	sprintf_s(tmp, "%04d-%02d-%02d %02d:%02d:%02d", aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec);
	int str_len = strlen(tmp);
	sprintf_s(tmp + str_len, sizeof(tmp) - str_len, " [%s] [%s@%d] ", LogLvToString(record.getLevel()), record.getFuncname(), (int)record.getLine());
	std::string logstr(tmp);
	logstr.append(std::move(record.message()));

	return logstr;
}


ColorConsoleAppender::ColorConsoleAppender()
{

}

void ColorConsoleAppender::write(const Record& record)
{
	std::string logstr = makeLogInfo(record);

	// lock
	std::lock_guard<std::mutex> lock(_lock);
	setColor(record.getLevel());
	std::cout << logstr <<std::endl;
	resetColor();
}

void ColorConsoleAppender::setColor(LogLevel loglv)
{
	switch (loglv)
	{
#ifdef _WIN32
	case fatal:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY | BACKGROUND_RED); // white on red background
		break;

	case error:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED); // red
		break;

	case warning:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_RED | FOREGROUND_GREEN); // yellow
		break;

	case debug:
	case verbose:
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_INTENSITY | FOREGROUND_GREEN | FOREGROUND_BLUE); // cyan
		break;
#else
	case fatal:
		std::cout << "\x1B[97m\x1B[41m"; // white on red background
		break;

	case error:
		std::cout << "\x1B[91m"; // red
		break;

	case warning:
		std::cout << "\x1B[93m"; // yellow
		break;

	case debug:
	case verbose:
		std::cout << "\x1B[96m"; // cyan
		break;
#endif
	default:
		break;
	}
}

void ColorConsoleAppender::resetColor()
{
#ifdef SYSTEM_WIN
	HANDLE hConsole = GetStdHandle(true ? STD_OUTPUT_HANDLE : STD_ERROR_HANDLE);
	SetConsoleTextAttribute(hConsole, FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);
#else
	fprintf((stdout_stream ? stdout : stderr), "\x1b[0m");
#endif
}