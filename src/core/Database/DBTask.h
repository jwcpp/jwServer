#pragma once

#include "../BaseType.h"

class MySqlConnect;
class DBTask
{
public:
	DBTask() : m_conn(nullptr) { }
	virtual ~DBTask() { }

	virtual bool execute() = 0;
	void SetConnection(MySqlConnect* con) { m_conn = con; }

protected:
	MySqlConnect* m_conn;
};


class DBPingTask : public DBTask
{
public:
	bool execute() override;
};


class SqlStatement;
class SqlResultSet;
#include <future>

using ResultSetPtr = std::shared_ptr<SqlResultSet>;


class QuickPrepare : public DBTask
{
public:
	QuickPrepare(std::shared_ptr<SqlStatement>& param);
	bool execute() override;
protected:
	std::shared_ptr<SqlStatement> m_sqlParam;
};


class AsyncResult
{
public:
	AsyncResult() :m_errno(0), m_result(nullptr) {}
	AsyncResult(uint32 error_, ResultSetPtr& result_) :m_errno(error_), m_result(std::move(result_)) {}
	AsyncResult(AsyncResult&& right) noexcept :m_errno(right.m_errno), m_result(std::move(right.m_result)) {}
	AsyncResult& operator = (AsyncResult&& right) noexcept {
		m_errno = right.m_errno;
		m_result = std::move(right.m_result);
		return *this;
	}


	uint32 getErrno() { return m_errno; }
	ResultSetPtr& getResult() { return m_result; }
protected:
	uint32 m_errno;
	ResultSetPtr m_result;
};

class SqlPrepareTask : public DBTask
{
public:
	SqlPrepareTask(std::shared_ptr<SqlStatement>& param, bool beResSet = false); //Need result set£¿
	bool execute() override;

	std::future<AsyncResult> getFuture() { return m_promise.get_future(); }
protected:
	bool m_beResSet;
	std::shared_ptr<SqlStatement> m_sqlParam;
	std::promise<AsyncResult> m_promise;
};

class ExecCallback
{
public:
	ExecCallback(std::future<AsyncResult>& future);
	bool complete();
	void addCallback(std::function<void(uint32, SqlResultSet*)> func);
	AsyncResult get();

protected:
	std::future<AsyncResult> m_future;
	std::function<void(uint32, SqlResultSet*)> m_backfunc;
};