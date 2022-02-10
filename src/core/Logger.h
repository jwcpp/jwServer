#pragma once
#include "IAppender.h"
#include "Record.h"

#include <vector>
#include <functional>
#include "asio/io_context_strand.hpp"
#include "Singleton.h"


template<int instanceId>
class Logger : public Util::Singleton<Logger<instanceId> >
{
public:
	Logger():_io(NULL), _strand(NULL)
	{
	
	}

	~Logger()
	{
		if (_strand)
		{
			delete _strand;
		}
	}

	void init(asio::io_context * io)
	{
		if (io && !_io)
		{
			_io = io;
			_strand = new asio::io_context::strand(*io);
		}
	}

	Logger& addAppender(IAppender* appender)
	{
		assert(appender);
		m_appenders.push_back(appender);
		return *this;
	}

	void operator+=(Record& record)
	{
		write(record);
	}

	void write(Record& record)
	{
		if (_io)
		{
			//_io->post(std::bind(&Logger<instanceId>::async_write, this, std::make_shared<Record>(std::move(record))));
			_strand->post(std::bind(&Logger<instanceId>::async_write, this, std::make_shared<Record>(std::move(record))));
		}
		else
		{
			_write(record);
		}
	}

protected:
	void async_write(std::shared_ptr<Record> record)
	{
		_write(*record);
	}

	void _write(Record& record)
	{
		for (std::vector<IAppender*>::iterator it = m_appenders.begin(); it != m_appenders.end(); ++it)
		{
			(*it)->write(record);
		}
	}

private:
	asio::io_context * _io;
	asio::io_context::strand* _strand;
    std::vector<IAppender*> m_appenders;
};

