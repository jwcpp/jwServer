#include "ServerTask.h"
#include "SessionMgr.h"
#include "XSocket.h"
#include "MsgProtocol.h"
#include "ServerDef.h"

ServerTask::ServerTask(const char* key):
	m_verify_key(key)
{

}

void ServerTask::onMsg(XSocketPtr& socket, MSGID msgid, NetPacket& packet)
{
	/*
	#define MSGID_ACCEPT 10
	#define MSGID_CONNECT 11
	#define MSGID_CLOSE 12
	*/
	
	switch (msgid)
	{
	case MSGID_ACCEPT:
		return;
	case MSGID_CLOSE:
		closeSession(socket);
		return;
	case MSGID_PING:
	{
		Session* session = (Session*)socket->getUser();
		if (session) {
			session->setOvertime(30);
			session->pong();
		}
	}
		return;
	case eMSGID_SessionVerify:
		verifySession(socket, packet);
		return;
	default:
		break;
	}

	Session* session = (Session*)socket->getUser();
	if (!session) return;

	m_handler.dispense(session, packet);
}

void ServerTask::verifySession(XSocketPtr& socket, NetPacket& packet)
{
	int type;
	packet >> type;
	std::string key;
	packet >> key;

	int ret = 0;
	do {
		Session* session = NULL;
		if (type == eST_Web)
		{
			session = SessionMgr::getInstance()->getWeb();
		}
		else if (type == eST_Game)
		{
			int serid = 0;
			packet >> serid;
			session = SessionMgr::getInstance()->getGame(serid);
			if(!session) break;

			printf("gameserver id:%d\n", serid);
		}
		else
		{
			ret = -100;
			break;
		}

		if (session->connectOk())
		{
			ret = -1;
			break;
		}
		if (key != m_verify_key)
		{
			ret = -2;
			break;
		}

		session->bindSocket(socket);
		// set overtime
		session->setOvertime(30);

	} while (0);

	NetPacket pack(eMSGID_SessionVerifyRet);
	pack << eST_Db;
	pack << ret;
	socket->sendPacket(pack);

	if (ret)socket->waitClose();
}

void ServerTask::closeSession(XSocketPtr& socket)
{
	//±»¶¯¹Ø±Õ
	Session* session = (Session*)socket->getUser();
	if (session) {
		session->clearSocket();
	}
}