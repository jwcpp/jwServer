#pragma once

#include "Util.h"
#include "LogDef.h"
#include <string>
#include <sstream>
#include <stdarg.h>

class Record
{
public:
	Record(const char* funcname, size_t line, LogLevel lv)
		:m_funcname(funcname), m_line(line), m_lv(lv)
	{
		m_time = time(NULL);
	}

	Record(Record && rec)
	{
		m_funcname = std::move(rec.m_funcname);
		m_line = rec.m_line;
		m_time = rec.m_time;
		m_lv = rec.m_lv;
		m_message = std::move(rec.m_message);
	}

	Record& ref()
	{
		return *this;
	}

	Record& operator<<(char data)
	{
		char str[] = { data, 0 };
		return *this << str;
	}

	template<typename T>
	Record& operator<<(const T& data)
	{
		m_message << data;
		return *this;
	}

	Record& printf(const char* format, ...)
	{
		va_list vl;
		va_start(vl, format);
		int len = vsnprintf(NULL, 0, format, vl);
		std::string str("", len + 1);
		vsprintf((char*)str.data(), format, vl);
		va_end(vl);

		*this << str;
		return *this;
	}

	std::string message() const
	{
		return m_message.str();
	}

	const size_t getLine() const { return m_line; }
	const char* getFuncname() const { return m_funcname.c_str(); }
	LogLevel getLevel() const { return m_lv; }
	const time_t * getTime() const { return &m_time; }

private:
	size_t					m_line;
	std::string				m_funcname;
	time_t					m_time;
	LogLevel				m_lv;
	std::ostringstream		m_message;
};
