#include "TableReader.h"
#include "TableParser.h"

bool TableReader::load(const char* file, char sep)
{
	TableParser parser;
	if (!parser.load(file, sep)) return false;

	return onLoad(&parser);
}

bool TableReader::reload(const char* file, char sep)
{
	TableParser parser;
	if (!parser.load(file, sep)) return false;

	return onReload(&parser);
}

bool TableReader::load(const std::string& file, char sep)
{
	return load(file.c_str(), sep);
}

bool TableReader::reload(const std::string& file, char sep)
{
	return reload(file.c_str(), sep);
}