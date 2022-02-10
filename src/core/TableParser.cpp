#include "TableParser.h"
#include <fstream>
#include "ISplit.h"

TableRow::TableRow():
	m_titles(NULL)
{

}

const char* TableRow::getField(const std::string& field)
{
	if (!m_titles) return NULL;

	auto it = m_titles->find(field);
	if (it == m_titles->end()) return NULL;

	return m_vecrow[it->second].c_str();
}

std::string TableRow::moveField(const std::string& field)
{
	if (!m_titles) return "";

	auto it = m_titles->find(field);
	if (it == m_titles->end()) return "";

	return std::move(m_vecrow[it->second]);
}

TableParser::TableParser():
	m_sep('\t')
{
	m_stream = new std::ifstream();
}

TableParser::~TableParser()
{
	if (m_stream)
	{
		delete m_stream;
	}
}

bool TableParser::load(const char* file, char sep)
{
	m_sep = sep;
	m_stream->open(file);
	if (!m_stream->is_open()) return false;
	
	return readHeader();
}

bool TableParser::readHeader()
{
	if (!m_stream) return false;

	std::string headers;
	if (!std::getline(*m_stream, headers))
	{
		return false;
	}

	std::stringstream ss(headers);
	std::string item;

	int idx = -1;
	while (std::getline(ss, item, m_sep))
	{
		idx++;
		if (item.empty()) continue;

		m_titles[item] = idx;
	}

	return true;
}

bool TableParser::getLine(TableRow* row)
{
	if (!m_stream) return false;

	std::string content;
	if (!std::getline(*m_stream, content))
	{
		return false;
	}
	
	if (row) {
		ISplit<> split(content, m_sep);
		row->setVecRow(split.move());
		row->setTitles(&m_titles);
	}
	return true;
}