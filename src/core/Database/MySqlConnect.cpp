#include "../Log.h"
#include "MySqlConnect.h"
#include "SqlStatement.h"
#include "SqlResultSet.h"
#include "MySqlCommon.h"
#include "DBTask.h"

MySqlConnect::MySqlConnect(SqlConfig* config, std::shared_ptr<ProducerConsumerQueue<DBTask*> > queue):
	m_pConfig(config), m_pThread(NULL), m_queue(queue ? queue : std::make_shared<ProducerConsumerQueue<DBTask*> >())
{
	if (!mysql_init(&m_mysql))
	{
		ERROR_LOG << "mysql init error!!";
	}
}

MySqlConnect::~MySqlConnect()
{
	mysql_close(&m_mysql);
	if (m_pThread)
	{
		delete m_pThread;
		m_pThread = NULL;
	}
}


bool MySqlConnect::open()
{
	if (mysql_options(&m_mysql, MYSQL_SET_CHARSET_NAME, "utf8"))
	{
		PRINT_ERROR("mysql_options(MYSQL_SET_CHARSET_NAME) Errno:%d failed: %s", getErrno(), getError());
		return false;
	}

	my_bool reconnect = 1;
	if (mysql_options(&m_mysql, MYSQL_OPT_RECONNECT, &reconnect))
	{
		PRINT_ERROR("mysql_options(MYSQL_OPT_RECONNECT) Errno:%d failed: %s", getErrno(), getError());
		return false;
	}

	if (!mysql_real_connect(&m_mysql, m_pConfig->m_ip.c_str(), m_pConfig->m_user.c_str(), m_pConfig->m_pswd.c_str(), m_pConfig->m_dbname.c_str(), m_pConfig->m_port, NULL, 0))
	{
		PRINT_ERROR("mysql_real_connect Errno:%d error: %s", getErrno(), getError());
		return false;
	}

	return true;
}

const char* MySqlConnect::getError()
{
	return mysql_error(&m_mysql);
}
int MySqlConnect::getErrno()
{
	return mysql_errno(&m_mysql);
}
void MySqlConnect::ping()
{
	mysql_ping(&m_mysql);
}

MYSQL* MySqlConnect::mysql()
{
	return &m_mysql;
}

bool MySqlConnect::_query(MYSQL_STMT* stmt, MYSQL_BIND* paramBind, MYSQL_RES ** pRes)
{
	if (paramBind)
	{
		if (mysql_stmt_bind_param(stmt, paramBind))
		{
			return false;
		}
	}

	if (mysql_stmt_execute(stmt))
	{
		return false;
	}

	*pRes = mysql_stmt_result_metadata(stmt);
	return true;
}

void MySqlConnect::start()
{
	assert(!m_pThread);
	m_pThread = new std::thread([this] {_run(); });
	m_isrun = true;
}

void MySqlConnect::stop()
{
	if (m_isrun)
	{
		m_isrun = false;
		if(m_pThread) m_pThread->join();
	}
	m_queue->Cancel();
}

void MySqlConnect::_run()
{
	while (m_isrun)
	{
		DBTask* task = NULL;
		m_queue->WaitAndPop(task);
		if (task)
		{
			task->SetConnection(this);
			if (task->execute())
			{

			}

			delete task;
		}
	}
}

bool MySqlConnect::execute(SqlStatement* statement)
{
	MYSQL_STMT* stmt = statement->prepare(&m_mysql);
	if (!stmt) return false;

	MYSQL_RES* pRes = NULL;
	if (!_query(stmt, statement->paramBind(), &pRes))
	{
		return false;
	}

	if (pRes)
	{
		mysql_free_result(pRes);
	}

	return true;
}

bool MySqlConnect::query(SqlStatement* statement, std::shared_ptr<SqlResultSet>& pResultSet)
{
	MYSQL_STMT* stmt = statement->prepare(&m_mysql);
	if (!stmt) return false;

	MYSQL_RES* pRes = NULL;
	if (!_query(stmt, statement->paramBind(), &pRes))
	{
		return false;
	}
	// (insert,delete,update) MYSQL_RES is NULL
	if (pRes)
	{
		std::shared_ptr<void> free_res(nullptr, [pRes](void*) { mysql_free_result(pRes); });

		if (mysql_stmt_store_result(stmt)) return false;

		// The select result is 0 rows
		uint64 rowcount = mysql_stmt_num_rows(stmt);
		if (rowcount > 0)
		{
			pResultSet = std::move(std::make_shared<SqlResultSet>());
			pResultSet->setResult(stmt, pRes, rowcount, mysql_stmt_field_count(stmt));
		}
	}

	return true;
}

void MySqlConnect::addTask(DBTask* task)
{
	m_queue->Push(task);
}