#pragma once

#include <string>
#include "mysql/mysql.h"
#include "../BaseType.h"

class SqlStatement
{
public:
	SqlStatement(const char* sqlstr);
	SqlStatement(int param);
	~SqlStatement();

	void setSql(std::string && sqlstr_) { m_sqlstr = std::move(sqlstr_); }
	int wpos() { return m_wpos; }
	void wpos(int pos) { m_wpos = pos; }
	MYSQL_BIND* paramBind() { return m_paramBind; }

	void pushInt8(int8 value);
	void pushUint8(uint8 value);
	void pushInt16(int16 value);
	void pushUint16(uint16 value);
	void pushInt32(int32 value);
	void pushUint32(uint32 value);
	void pushInt64(int64 value);
	void pushUint64(uint64 value);
	void pushFloat(float value);
	void pushDouble(double value);
	void pushString(std::string value);
	void pushData(std::string_view sv);
	void write(int pos, void* pData, int len);

	MYSQL_STMT * prepare(MYSQL* mysql);
	MYSQL_STMT* getStmt() { return m_stmt; }
protected:
	void init(int param);
private:
	int m_wpos;
	int m_count;
	std::string m_sqlstr;
	MYSQL_BIND* m_paramBind;
	unsigned long* m_paramLengths;
	MYSQL_STMT* m_stmt;
};

