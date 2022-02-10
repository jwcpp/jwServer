#pragma once

#include "ServerApp.h"
class SocketMgr;
class HttpServer;
class HttpClientMgr;
class HttpMsg;
class ConnectTask;
class SessionMgr;
class WebServer : public ServerApp
{
public:
	int init();
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
	HttpServer* m_httpserver = NULL;
	HttpClientMgr* m_httpclientmgr = NULL;
	HttpMsg* m_httpmsg = NULL;
	ConnectTask* m_task = NULL;
	SessionMgr* m_sessionMgr = NULL;
};

