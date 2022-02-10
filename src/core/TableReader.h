#pragma once

#include <string>

class TableParser;
class TableReader
{
public:
	virtual ~TableReader() {}

	bool load(const char * file, char sep = '\t');
	bool reload(const char * file, char sep = '\t');
	bool load(const std::string & file, char sep = '\t');
	bool reload(const std::string & file, char sep = '\t');
protected:
	virtual bool onLoad(TableParser * parser) = 0;
	virtual bool onReload(TableParser* parser) = 0;

};

