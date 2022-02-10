#include "FileAppender.h"
#include "Util.h"
#include <stdio.h>

FileAppender::FileAppender(const char* filename, int maxsize):
	_filename(filename),
	_maxsize(maxsize),
	_file(NULL),
	_currday(0),
	_currsize(0)
{

}

FileAppender::~FileAppender()
{
	closeFile();
}

extern std::string makeLogInfo(const Record& record);
void FileAppender::write(const Record& record)
{
	tm aTm;
	time_t t = time(0);
	localtime_r(&t, &aTm);
	std::string logstr = makeLogInfo(record);
	{
		std::lock_guard<std::mutex> lock(_lock);
		if (aTm.tm_mday != _currday)
		{
			_currday = aTm.tm_mday;

			std::string filename = _filename;
			char tmp[32];
			sprintf_s(tmp, "_%04d%02d%02d_%d.txt", aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, Util::getPid());
			filename.append(tmp);

			closeFile();
			_currsize = openFile(filename.c_str());
		}
		
		if(_currsize >= _maxsize)
		{
			std::string filename = _filename;
			char tmp[64];
			sprintf_s(tmp, "_%04d%02d%02d-%02d.%02d.%02d_%d.txt", aTm.tm_year + 1900, aTm.tm_mon + 1, aTm.tm_mday, aTm.tm_hour, aTm.tm_min, aTm.tm_sec, Util::getPid());
			filename.append(tmp);

			closeFile();
			_currsize = openFile(filename.c_str());
		}

		if (!_file) return;
		_currsize += fprintf_s(_file, "%s\n", logstr.c_str());
		fflush(_file);
	}
}

int FileAppender::openFile(const char* file)
{
	fopen_s(&_file, file, "ab+");
	if (_file)
	{
		fseek(_file, 0, SEEK_END);
		return (int)ftell(_file);
	}

	return 0;
}

void FileAppender::closeFile()
{
	if (_file)
	{
		fclose(_file);
		_file = NULL;
		_currsize = 0;
	}
}