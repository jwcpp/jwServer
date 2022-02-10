#include "Database.h"
#include "../Util.h"
#include "MySqlConnect.h"
#include "DBTask.h"

Database::Database(const char* host, const char* dbname, const char* user, const char* pswd, unsigned int port)
{
	m_config.m_ip = host;
	m_config.m_port = port;
	m_config.m_dbname = dbname;
	m_config.m_user = user;
	m_config.m_pswd = pswd;
}

Database::~Database()
{
	for (int i = 0; i < m_connects.size(); ++i)
	{
		delete m_connects[i];
	}
}

void Database::create(int count)
{
#ifdef Mysql_Concurrent_Queue
	m_queue = std::make_shared<ProducerConsumerQueue<DBTask*> >();
	for (int i = 0; i < count; ++i)
	{
		m_connects.push_back(new MySqlConnect(&m_config, m_queue));
	}
#else

	for (int i = 0; i < count; ++i)
	{
		auto queueptr = std::make_shared<ProducerConsumerQueue<DBTask*> >()
		m_connects.push_back(new MySqlConnect(&m_config, queueptr));
	}
#endif
}

bool Database::open()
{
	for (int i = 0; i < m_connects.size(); ++i)
	{
		if (!m_connects[i]->open())
		{
			return false;
		}
	}

	return true;
}
void Database::start()
{
	for (int i = 0; i < m_connects.size(); ++i)
	{
		m_connects[i]->start();
	}
}

void Database::stop()
{
	for (int i = 0; i < m_connects.size(); ++i)
	{
		m_connects[i]->stop();
	}

#ifdef Mysql_Concurrent_Queue
	//m_queue->Cancel();
#endif
}


MySqlConnect* Database::get(int threadidx)
{
	if (threadidx < 0 || threadidx >= count())
	{
		return NULL;
	}

	return m_connects[threadidx];
}

MySqlConnect* Database::getRandom()
{
	int threadidx = Util::random(0, count() - 1);
	return m_connects[threadidx];
}

#ifdef Mysql_Concurrent_Queue

void Database::asyncQuick(std::shared_ptr<SqlStatement>& param)
{
	QuickPrepare* task = new QuickPrepare(param);
	auto conn = m_connects[0];
	conn->addTask(task);
}

std::shared_ptr<ExecCallback> Database::asyncExec(std::shared_ptr<SqlStatement>& param)
{
	SqlPrepareTask* task = new SqlPrepareTask(param);
	std::future<AsyncResult> future_ = task->getFuture();

	auto conn = m_connects[0];
	conn->addTask(task);

	return std::make_shared<ExecCallback>(future_);
}

std::shared_ptr<ExecCallback> Database::asyncQuery(std::shared_ptr<SqlStatement>& param)
{
	SqlPrepareTask* task = new SqlPrepareTask(param, true);
	std::future<AsyncResult> future_ = task->getFuture();

	auto conn = m_connects[0];
	conn->addTask(task);

	return std::make_shared<ExecCallback>(future_);
}

#else

void Database::asyncQuick(std::shared_ptr<SqlStatement> & param, int threadidx)
{
	QuickPrepare* task = new QuickPrepare(param);
	auto conn = get(threadidx);
	if (!conn) conn = getRandom();
	conn->addTask(task);
}

std::shared_ptr<ExecCallback> Database::asyncExec(std::shared_ptr<SqlStatement> & param, int threadidx)
{
	SqlPrepareTask* task = new SqlPrepareTask(param);
	std::future<AsyncResult> future_ = task->getFuture();

	auto conn = get(threadidx);
	if (!conn) conn = getRandom();
	conn->addTask(task);

	return std::make_shared<ExecCallback>(future_);
}

std::shared_ptr<ExecCallback> Database::asyncQuery(std::shared_ptr<SqlStatement> & param, int threadidx)
{
	SqlPrepareTask* task = new SqlPrepareTask(param, true);
	std::future<AsyncResult> future_ = task->getFuture();
	
	auto conn = get(threadidx);
	if (!conn) conn = getRandom();
	conn->addTask(task);

	return std::make_shared<ExecCallback>(future_);
}

#endif