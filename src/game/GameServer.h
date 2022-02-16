#pragma once

#include "ServerApp.h"
class SocketMgr;
class HttpMsg;
class ConnectTask;
class SessionMgr;
class StartMgr;
class Accept;
class C2ServerTask;
class GameServer : public ServerApp
{
public:
	int init(int argc, char* argv[]);
	void stop();
	void release();

protected:
	virtual void onUpdate(uint32 diff);
	virtual void onTick(uint32 diff);
	virtual void onSecond();
	virtual void onMinute();
	virtual void onHour();
	virtual void onDay();

private:
	SocketMgr* m_socketmgr = NULL;
	ConnectTask* m_task = NULL;
	SessionMgr* m_sessionMgr = NULL;
	StartMgr* m_startMgr = NULL;
	Accept* m_accept = NULL;
	C2ServerTask* m_c2sTask = NULL;
};

