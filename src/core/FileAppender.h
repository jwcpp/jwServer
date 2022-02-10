#pragma once
#include "IAppender.h"
#include <string>
#include <mutex>

class FileAppender :
    public IAppender
{
public:
    FileAppender(const char * filename, int maxsize);
    virtual ~FileAppender();

    virtual void write(const Record& record);

protected:
    int openFile(const char * file);
    void closeFile();
private:
    std::string _filename;
    int _maxsize;
    int _currsize;
    int _currday;
    FILE* _file;
    std::mutex _lock;
};

