#include "GameServer.h"
#include "ServerConfig.h"
#include "SocketMgr.h"
#include "XSocket.h"
#include "ConnectTask.h"
#include "SessionMgr.h"
#include "StartMgr.h"

int GameServer::init(int argc, char* argv[])
{
	ServerConfig config;
	if (config.load("server.xml")) return -1;

	m_startMgr = new StartMgr;
	if (m_startMgr->init()) return -2;
	
	if (argc < 2) return -3;
	int serverid = atoi(argv[1]);
	if (!config.gameAddr(serverid)) return -4;

	m_socketmgr = new SocketMgr(1);
	m_task = new ConnectTask(serverid, config.verifyKey());
	m_sessionMgr = new SessionMgr;

	// connect db
	std::shared_ptr<XSocket> dbsocket(std::make_shared<XSocket>(m_socketmgr->next(), m_task));
	dbsocket->connect(config.dbAddr()->ip.c_str(), config.dbAddr()->port);

	m_socketmgr->start();
	return 0;
}

void GameServer::stop()
{
	if (m_socketmgr) m_socketmgr->stop();
	if (m_startMgr)m_startMgr->stop();
}

void GameServer::release()
{
	if (m_socketmgr) delete m_socketmgr;
	if (m_task) delete m_task;
	if (m_sessionMgr) delete m_sessionMgr;
	if (m_startMgr) {
		m_startMgr->release();
		delete m_startMgr;
	}
}

void GameServer::onUpdate(uint32 diff)
{
	m_task->update();
	m_startMgr->onUpdate(diff);
}
void GameServer::onTick(uint32 diff)
{
	m_startMgr->onTick(diff);
}
void GameServer::onSecond()
{
	m_sessionMgr->updateSec();
	m_startMgr->onSecond();

}
void GameServer::onMinute()
{
	m_startMgr->onMinute();
}
void GameServer::onHour()
{
	m_startMgr->onHour();
}
void GameServer::onDay()
{
	m_startMgr->onDay();
}