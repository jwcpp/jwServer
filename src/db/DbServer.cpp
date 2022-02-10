#include "DbServer.h"
#include "ServerConfig.h"
#include "Accept.h"
#include "SocketMgr.h"
#include "XSocket.h"
#include "ServerTask.h"
#include "SessionMgr.h"
#include "DBMgr.h"

int DbServer::init()
{
	ServerConfig config;
	if (config.load("server.xml")) return -1;

	m_dbMgr = new DBMgr();
	if (m_dbMgr->init(&config))
	{
		return -1;
	}

	m_socketmgr = new SocketMgr(1);
	m_sessionMgr = new SessionMgr;
	m_sessionMgr->init<>(config.games());

	m_task = new ServerTask(config.verifyKey());
	m_accept = new Accept([this]()->SocketPtr {
		return std::make_shared<XSocket>(m_socketmgr->next(), m_task);
		});

	m_accept->start(config.dbAddr()->ip.c_str(), config.dbAddr()->port, m_socketmgr->next());

	m_socketmgr->start();
	return 0;
}

void DbServer::stop()
{
	if (m_accept) m_accept->close();
	if (m_socketmgr)m_socketmgr->stop();
	if (m_dbMgr) m_dbMgr->stop();
}

void DbServer::release()
{
	if (m_dbMgr) {
		m_dbMgr->release();
		delete m_dbMgr;
	}
	if (m_socketmgr) delete m_socketmgr;
	if (m_sessionMgr) delete m_sessionMgr;
	if (m_task) delete m_task;
	if (m_accept)delete m_accept;
}

void DbServer::onUpdate(uint32 diff)
{
	m_task->update();
}

void DbServer::onTick(uint32 diff)
{
	m_dbMgr->tick();
}

void DbServer::onSecond()
{
	m_sessionMgr->updateSec();
	m_dbMgr->updateSec();
}