#pragma once

#include <time.h>
#include "BaseType.h"
#include <string>
#include <random>

struct tm* localtime_r(time_t const* time, struct tm* result);

namespace Util
{
	uint32 getTid();
	uint32 getPid();

	void localtime_s(struct tm* t, const time_t* time);
	void gmtime_s(struct tm* t, const time_t* time); // utc time

	std::string timeToTimestampStr(time_t t);

	int charCount(char* pStr, char c);

	extern std::mt19937& getMtGen();
	template<typename T>
	T random(T min, T max){
		std::uniform_int_distribution<> dis(min, max);
		return dis(getMtGen());
	}

	int mkdir(const char* dir);
};

