#pragma once

#include "mysql/mysql.h"
#include <map>
#include <thread>
#include "../ProducerConsumerQueue.h"
#include <string>

class DBTask;
class SqlStatement;
class SqlResultSet;
struct SqlConfig;
class MySqlConnect
{
public:
	MySqlConnect(SqlConfig* config, std::shared_ptr<ProducerConsumerQueue<DBTask*> > queue = nullptr);
	virtual ~MySqlConnect();

	bool open();
	const char* getError();
	int getErrno();
	void ping();
	MYSQL* mysql();

	void start();
	void stop();
	bool execute(SqlStatement* param);
	bool query(SqlStatement* param, std::shared_ptr<SqlResultSet> & pResultSet);
	void addTask(DBTask * task);
protected:
	void _run();
	bool _query(MYSQL_STMT* stmt, MYSQL_BIND * paramBind, MYSQL_RES ** pRes);
private:
	MYSQL m_mysql;
	SqlConfig* m_pConfig;
	std::thread * m_pThread;
	std::atomic<bool> m_isrun{ true };
	std::shared_ptr<ProducerConsumerQueue<DBTask*> > m_queue;
};

