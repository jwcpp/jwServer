#include "WebServer.h"
#include "ServerConfig.h"
#include "SocketMgr.h"
#include "HttpServer.h"
#include "HttpClientMgr.h"
#include "HttpMsg.h"
#include "XSocket.h"
#include "ConnectTask.h"
#include "SessionMgr.h"

int WebServer::init()
{
	ServerConfig config;
	if (config.load("server.xml")) return -1;

	m_socketmgr = new SocketMgr(1);
	m_httpmsg = new HttpMsg;
	m_task = new ConnectTask(config.verifyKey());
	m_sessionMgr = new SessionMgr;
	
	// http client
	m_httpclientmgr = new HttpClientMgr(m_socketmgr->next()->getIoPtr());
	/*
	HttpClientMgr::getInstance()->http_get("192.168.10.66:80", "/test", "", [](std::shared_ptr<HttpClient::Response> response, int ec) {
		printf("%d %s\n", ec, response->content.string().c_str());
		});
	*/

	// http server
	m_httpserver = new HttpServer(m_socketmgr->next()->getIoPtr());
	m_httpserver->run(config.httpAddr()->ip.c_str(), config.httpAddr()->port);
	m_httpmsg->registerHttp(m_httpserver);


	// connect db
	std::shared_ptr<XSocket> dbsocket(std::make_shared<XSocket>(m_socketmgr->next(), m_task));
	dbsocket->connect(config.dbAddr()->ip.c_str(), config.dbAddr()->port);

	m_socketmgr->start();
	return 0;
}

void WebServer::stop()
{
	if (m_httpserver)m_httpserver->stop();
	if (m_socketmgr) m_socketmgr->stop();
}

void WebServer::release()
{
	if (m_socketmgr) delete m_socketmgr;
	if (m_httpserver) delete m_httpserver;
	if (m_httpclientmgr) delete m_httpclientmgr;
	if (m_httpmsg) delete m_httpmsg;
	if (m_task) delete m_task;
	if (m_sessionMgr) delete m_sessionMgr;
}

void WebServer::onUpdate(uint32 diff)
{
	m_task->update();
}
void WebServer::onTick(uint32 diff)
{
	m_httpserver->update();
	m_httpclientmgr->update();
}
void WebServer::onSecond()
{
	m_sessionMgr->updateSec();
	m_httpmsg->updateSec();
}
void WebServer::onMinute()
{

}
void WebServer::onHour()
{

}
void WebServer::onDay()
{

}