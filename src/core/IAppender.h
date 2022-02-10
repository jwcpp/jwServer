#pragma once

class Record;
class IAppender
{
public:
	virtual ~IAppender()
	{
	}

	virtual void write(const Record& record) = 0;
};