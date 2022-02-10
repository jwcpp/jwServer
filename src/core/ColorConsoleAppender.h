#pragma once
#include "IAppender.h"
#include "LogDef.h"
#include <mutex>

class ColorConsoleAppender :public IAppender
{
public:
	ColorConsoleAppender();

	virtual void write(const Record& record);
protected:
	void setColor(LogLevel loglv);
	void resetColor();

private:
	std::mutex _lock;
};

