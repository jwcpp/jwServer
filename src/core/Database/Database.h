#pragma once

#include <vector>
#include <memory>
#include "MySqlCommon.h"
#include "ProducerConsumerQueue.h"

class ExecCallback;
class MySqlConnect;
class SqlStatement;
class DBTask;

class Database
{
public:
	Database(const char* host, const char* dbname, const char* user, const char* pswd = "", unsigned int port = 3306);
	~Database();

	void create(int count);
	bool open();
	void start();
	void stop();

	int count() { return m_connects.size(); }
	MySqlConnect* get(int threadidx);
	MySqlConnect* getRandom();

#ifdef Mysql_Concurrent_Queue
	void asyncQuick(std::shared_ptr<SqlStatement>& param);
	std::shared_ptr<ExecCallback> asyncExec(std::shared_ptr<SqlStatement>& param);
	std::shared_ptr<ExecCallback> asyncQuery(std::shared_ptr<SqlStatement>& param);
#else
	void asyncQuick(std::shared_ptr<SqlStatement> & param, int threadidx = -1);
	std::shared_ptr<ExecCallback> asyncExec(std::shared_ptr<SqlStatement> & param, int threadidx = -1);
	std::shared_ptr<ExecCallback> asyncQuery(std::shared_ptr<SqlStatement> & param, int threadidx = -1);
#endif

protected:
	std::vector<MySqlConnect*> m_connects;
	SqlConfig m_config;

#ifdef Mysql_Concurrent_Queue
	std::shared_ptr<ProducerConsumerQueue<DBTask*> > m_queue;
#endif
};

