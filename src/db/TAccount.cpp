#include "TAccount.h"
#include "DbGlobal.h"
#include "Database/Database.h"
#include "Database/SqlStatement.h"
#include "Database/DBTask.h"
#include "Database/SqlResultSet.h"

void TAccount::load()
{
	auto sqlptr = std::make_shared<SqlStatement>("select * from account");
	auto callback = randomDatabase()->asyncQuery(sqlptr);

	//Sleep(1000 * 10);
	AsyncResult result = callback->get(); //wait
	ResultSetPtr resultSet = result.getResult();
	
	while (resultSet && resultSet->fetch())
	{
		auto id = (*resultSet)[0].getValue<uint64_t>();
		auto user = (*resultSet)[1].str();
		auto pass = (*resultSet)[2].str();

		TAccountinfo info;
		info.id = id;
		info.pass = std::move(pass);
		m_datas[std::move(user)] = info;

		if (m_incId < id) m_incId = id;
	}
}

TAccountinfo* TAccount::getInfo(std::string& user)
{
	auto it = m_datas.find(user);
	if (it == m_datas.end())return NULL;

	return &(it->second);
}

uint64_t TAccount::creatAccount(std::string& user, std::string& pass)
{
	TAccountinfo info;
	info.id = ++m_incId;
	info.pass = pass;
	m_datas[user] = info;

	//save db
	auto ptr = std::make_shared<SqlStatement>("REPLACE INTO account(id,account,pass) VALUES(?,?,?)");
	ptr->pushUint64(m_incId);
	ptr->pushString(user);
	ptr->pushString(pass);
	writeDatabase()->asyncExec(ptr);

	return m_incId;
}