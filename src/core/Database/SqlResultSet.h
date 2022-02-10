#pragma once

#include <stdio.h>
#include <string>

class SqlField
{
public:
	SqlField()
	{
		m_buff = NULL;
		m_len = 0;
	}

	~SqlField()
	{
		if (m_buff)
		{
			delete[] m_buff;
		}
	}

	void make(const char* pdata, int len)
	{
		m_len = len;
		m_buff = new char[len];
		memcpy(m_buff, pdata, len);
	}

	bool isEmpty()
	{
		return !m_buff;
	}

	const char* data() { return m_buff; }
	int length() { return m_len; }

	template<typename T>
	T getValue()
	{
		T val;
		memcpy(&val, m_buff, sizeof(T));
		return val;
	}

	std::string str() { return std::string(m_buff, m_len); }

private:
	char* m_buff;
	int m_len;
};

#include <vector>
#include "../BaseType.h"

class SqlResultSet
{
public:
	SqlResultSet();
	~SqlResultSet();
	
	void setResult(MYSQL_STMT* stmt, MYSQL_RES* pResult, uint64 pRowCount, uint32 pFieldCount);
	bool fetch();
	bool isEmpty();
	uint32 getRowCount();
	uint32 getFieldsCount();
	const char* getData(int idx, int& len);
	const char* getData(int idx);

	SqlField* getField(int idx);
	SqlField & operator[](int idx);
	
protected:
	bool _fetch(MYSQL_STMT * stmt);
private:
	std::vector<SqlField> m_fields;
	int64 m_currRowIdx;
	int64 m_rowCount;
	int32 m_fieldCount;
};

