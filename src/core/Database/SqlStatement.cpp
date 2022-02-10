#include <assert.h>
#include "SqlStatement.h"
#include "../Util.h"

SqlStatement::SqlStatement(const char* sqlstr):
	m_sqlstr(sqlstr), m_paramBind(NULL), m_count(0), m_paramLengths(NULL), m_wpos(0), m_stmt(NULL)
{
	init(Util::charCount((char*)sqlstr, '?'));
}

SqlStatement::SqlStatement(int param):
	m_paramBind(NULL), m_count(0), m_paramLengths(NULL), m_wpos(0), m_stmt(NULL)
{
	init(param);
}

SqlStatement::~SqlStatement()
{
	if (m_paramBind)
	{
		for (int i = 0; i < m_count; ++i)
		{
			MYSQL_BIND* pBind = &m_paramBind[i];
			if (pBind->buffer) delete[] pBind->buffer;
		}

		delete[] m_paramBind;
		delete[] m_paramLengths;
		m_paramBind = NULL;
		m_paramLengths = NULL;
	}

	if (m_stmt)
	{
		mysql_stmt_close(m_stmt);
		m_stmt = NULL;
	}
}

void SqlStatement::init(int param)
{
	m_count = param;
	if (m_count > 0)
	{
		m_paramBind = new MYSQL_BIND[m_count];
		memset(m_paramBind, 0, sizeof(MYSQL_BIND) * m_count);
		m_paramLengths = new unsigned long[m_count];
		for (int i = 0; i < m_count; ++i)
		{
			m_paramBind[i].length = &m_paramLengths[i];
		}
	}
}

static void SetParameterValue(MYSQL_BIND* param, enum_field_types type, const void* value, uint32 len, bool isUnsigned)
{
	if (param->buffer) delete[] param->buffer;

	param->buffer_type = type;
	param->buffer = new char[len];
	param->buffer_length = *param->length = len;
	param->is_null_value = 0;
	param->is_unsigned = isUnsigned;

	memcpy(param->buffer, value, len);
}

#define CHECK_WRITE_IDX assert(m_wpos < m_count)
void SqlStatement::pushInt8(int8 value)
{
	CHECK_WRITE_IDX;
	MYSQL_BIND* pBind = &m_paramBind[m_wpos++];
	SetParameterValue(pBind, MYSQL_TYPE_TINY, &value, sizeof(value), false);
}

void SqlStatement::pushUint8(uint8 value)
{
	CHECK_WRITE_IDX;
	MYSQL_BIND* pBind = &m_paramBind[m_wpos++];
	SetParameterValue(pBind, MYSQL_TYPE_TINY, &value, sizeof(value), true);
}
void SqlStatement::pushInt16(int16 value)
{
	CHECK_WRITE_IDX;
	MYSQL_BIND* pBind = &m_paramBind[m_wpos++];
	SetParameterValue(pBind, MYSQL_TYPE_SHORT, &value, sizeof(value), false);
}
void SqlStatement::pushUint16(uint16 value)
{
	CHECK_WRITE_IDX;
	MYSQL_BIND* pBind = &m_paramBind[m_wpos++];
	SetParameterValue(pBind, MYSQL_TYPE_SHORT, &value, sizeof(value), true);
}
void SqlStatement::pushInt32(int32 value)
{
	CHECK_WRITE_IDX;
	MYSQL_BIND* pBind = &m_paramBind[m_wpos++];
	SetParameterValue(pBind, MYSQL_TYPE_LONG, &value, sizeof(value), false);
}
void SqlStatement::pushUint32(uint32 value)
{
	CHECK_WRITE_IDX;
	MYSQL_BIND* pBind = &m_paramBind[m_wpos++];
	SetParameterValue(pBind, MYSQL_TYPE_LONG, &value, sizeof(value), true);
}
void SqlStatement::pushInt64(int64 value)
{
	CHECK_WRITE_IDX;
	MYSQL_BIND* pBind = &m_paramBind[m_wpos++];
	SetParameterValue(pBind, MYSQL_TYPE_LONGLONG, &value, sizeof(value), false);
}
void SqlStatement::pushUint64(uint64 value)
{
	CHECK_WRITE_IDX;
	MYSQL_BIND* pBind = &m_paramBind[m_wpos++];
	SetParameterValue(pBind, MYSQL_TYPE_LONGLONG, &value, sizeof(value), true);
}
void SqlStatement::pushFloat(float value)
{
	CHECK_WRITE_IDX;
	MYSQL_BIND* pBind = &m_paramBind[m_wpos++];
	SetParameterValue(pBind, MYSQL_TYPE_FLOAT, &value, sizeof(value), (value > 0.0f));
}
void SqlStatement::pushDouble(double value)
{
	CHECK_WRITE_IDX;
	MYSQL_BIND* pBind = &m_paramBind[m_wpos++];
	SetParameterValue(pBind, MYSQL_TYPE_DOUBLE, &value, sizeof(value), (value > 0.0f));
}
void SqlStatement::pushString(std::string value)
{
	CHECK_WRITE_IDX;
	MYSQL_BIND* pBind = &m_paramBind[m_wpos++];

	int len = value.length();

	pBind->buffer_length = *pBind->length = len;
	pBind->buffer_type = MYSQL_TYPE_STRING;
	pBind->is_unsigned = 0;

	if (pBind->buffer) delete[] pBind->buffer;
	pBind->buffer = new char[len];
	memcpy(pBind->buffer, value.data(), len);
}
void SqlStatement::pushData(std::string_view sv)
{
	write(m_wpos++, (void*)sv.data(), sv.size());
}
void SqlStatement::write(int pos, void* pData, int len)
{
	assert(pos < m_count);

	MYSQL_BIND* pBind = &m_paramBind[pos];

	pBind->buffer_length = *pBind->length = len;
	pBind->buffer_type = MYSQL_TYPE_BLOB;
	pBind->is_unsigned = 0;

	if (pBind->buffer) delete[] pBind->buffer;
	
	pBind->buffer = new char[len];
	memcpy(pBind->buffer, pData, len);
}

MYSQL_STMT * SqlStatement::prepare(MYSQL* mysql)
{
	assert(!m_stmt);
	m_stmt = mysql_stmt_init(mysql);
	if (!m_stmt)
	{
		return NULL;
	}

	if (mysql_stmt_prepare(m_stmt, m_sqlstr.c_str(), static_cast<unsigned long>(m_sqlstr.size())))
	{
		return NULL;
	}

	if (static_cast<int>(mysql_stmt_param_count(m_stmt)) != m_count)
	{
		return NULL;
	}

	my_bool bool_tmp = my_bool(1);
	mysql_stmt_attr_set(m_stmt, STMT_ATTR_UPDATE_MAX_LENGTH, &bool_tmp);

	return m_stmt;
}