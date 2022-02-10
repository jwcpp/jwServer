#pragma once

#define COMMON_INSTANCE_ID 0

enum LogLevel
{
	none = 0,
	fatal = 1,
	error = 2,
	warning = 3,
	info = 4,
	debug = 5,
	verbose = 6
};

inline const char* LogLvToString(LogLevel type)
{
	switch (type)
	{
	case fatal:
		return "FATAL";
	case error:
		return "ERROR";
	case warning:
		return "WARN";
	case info:
		return "INFO";
	case debug:
		return "DEBUG";
	case verbose:
		return "VERB";
	default:
		return "NONE";
	}
}