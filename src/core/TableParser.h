#pragma once

#include <map>
#include <string>
#include <vector>
#include <sstream>

class TableRow
{
public:
	TableRow();

	template<typename T>
	T get(const std::string& field, T defvalue = T())
	{
		T res = defvalue;
		std::stringstream ss(getField(field));
		ss >> res;
		return res;
	}

	const char * getField(const std::string& field);
	std::string moveField(const std::string& field);

	void setVecRow(std::vector<std::string>&& vec) { m_vecrow = std::move(vec); };
	//std::vector<std::string> & getVecRow() { return m_vecrow; }
	void setTitles(std::map<std::string, int>* titles) { m_titles = titles; }

private:
	TableRow(const TableRow&);
	TableRow& operator=(const TableRow&);
private:
	std::map<std::string, int> * m_titles;
	std::vector<std::string> m_vecrow;
};

class TableParser
{
public:
	TableParser();
	~TableParser();

	bool load(const char * file, char sep = '\t');
	bool getLine(TableRow * row = NULL);
private:
	TableParser(const TableParser&);
	TableParser& operator=(const TableParser&);

	bool readHeader();
private:
	char m_sep;
	std::ifstream * m_stream;
	std::map<std::string, int> m_titles;
};