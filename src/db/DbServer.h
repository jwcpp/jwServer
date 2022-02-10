#pragma once

#include "ServerApp.h"

class DBMgr;
class SocketMgr;
class Accept;
class ServerTask;
class SessionMgr;
class DbServer : public ServerApp
{
public:
	int init();
	void stop();
	void release();

protected:
	virtual void onUpdate(uint32 diff);
	virtual void onTick(uint32 diff);
	virtual void onSecond();

private:
	DBMgr* m_dbMgr = NULL;
	SocketMgr* m_socketmgr = NULL;
	Accept* m_accept = NULL;
	ServerTask* m_task = NULL;
	SessionMgr* m_sessionMgr = NULL;
};
