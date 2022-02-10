#include "DBTask.h"
#include "MySqlConnect.h"
#include "SqlStatement.h"
#include "SqlResultSet.h"

bool DBPingTask::execute()
{
	m_conn->ping();
	return true;
}

QuickPrepare::QuickPrepare(std::shared_ptr<SqlStatement>& param):
	m_sqlParam(std::move(param))
{

}

bool QuickPrepare::execute()
{
	return m_conn->execute(m_sqlParam.get());
}

SqlPrepareTask::SqlPrepareTask(std::shared_ptr<SqlStatement>& param, bool beResSet):
	m_sqlParam(std::move(param)),
	m_beResSet(beResSet)
{
	
}

bool SqlPrepareTask::execute()
{
	bool success = false;
	ResultSetPtr resPtr = nullptr;
	if (!m_beResSet)
	{
		success = m_conn->execute(m_sqlParam.get());
	}
	else
	{
		success = m_conn->query(m_sqlParam.get(), resPtr);
	}

	uint32 errno_ = 0;
	if (!success)
	{
		errno_ = m_conn->getErrno();
	}

	AsyncResult res_obj(errno_, resPtr);
	m_promise.set_value(std::move(res_obj));

	return success;
}


ExecCallback::ExecCallback(std::future<AsyncResult>& future) :
	m_future(std::move(future))
{

}

bool ExecCallback::complete()
{
	if (m_future.wait_for(std::chrono::seconds(0)) == std::future_status::ready)
	{
		AsyncResult result = m_future.get();
		ResultSetPtr& resptr = result.getResult();
		if (m_backfunc) {
			m_backfunc(result.getErrno(), resptr ? resptr.get() : NULL);
		}
		return true;
	}

	return false;
}

void ExecCallback::addCallback(std::function<void(uint32, SqlResultSet*)> func)
{
	m_backfunc = func;
}

AsyncResult ExecCallback::get()
{
	return m_future.get();
}