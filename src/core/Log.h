#pragma once

#include "Logger.h"

#define WRITE_LOG(level) (*Logger<COMMON_INSTANCE_ID>::getInstance()) += Record(__FUNCTION__, __LINE__, level).ref()

#define INFO_LOG	WRITE_LOG(LogLevel::info)
#define WARNING_LOG WRITE_LOG(LogLevel::warning)
#define ERROR_LOG	WRITE_LOG(LogLevel::error)
#define XLOG(level) WRITE_LOG(level)
#define XLOG_IF(condition, level) if(!(condition)){;} else WRITE_LOG(level)

#define EXTLOG(id, level) (*Logger<id>::getInstance()) += Record(__FUNCTION__, __LINE__, level).ref()


#define PRINTF_LOG(level, ...) (*Logger<COMMON_INSTANCE_ID>::getInstance()) += Record(__FUNCTION__, __LINE__, level).printf(__VA_ARGS__)

#define PRINT_INFO(...) PRINTF_LOG(LogLevel::info, __VA_ARGS__)
#define PRINT_WARNING(...) PRINTF_LOG(LogLevel::warning, __VA_ARGS__)
#define PRINT_ERROR(...) PRINTF_LOG(LogLevel::error, __VA_ARGS__)