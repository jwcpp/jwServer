#include "ConnectTask.h"
#include "SessionMgr.h"
#include "XSocket.h"
#include "MsgProtocol.h"
#include "ServerDef.h"

ConnectTask::ConnectTask(const char* key):
	m_verify_key(key)
{

}
void ConnectTask::onMsg(XSocketPtr& socket, MSGID msgid, NetPacket& packet)
{
	/*
	#define MSGID_ACCEPT 10
	#define MSGID_CONNECT 11
	#define MSGID_CLOSE 12
	*/
	
	switch (msgid)
	{
	case MSGID_CONNECT:
	{
		int ret;
		packet >> ret;
		if (!ret)
		{
			NetPacket pack(eMSGID_SessionVerify);
			pack << eST_Web;
			pack << m_verify_key;
			socket->sendPacket(pack);
			printf("connect ok!\n");
		}
		else
		{
			printf("connect fail!\n");
			socket->connect(socket->GetRemoteIpAddress(), socket->GetRemotePort());
		}
	}
		return;
	case MSGID_CLOSE:
		closeSession(socket);
		return;

	case MSGID_PONG:
	{
		ActiveSession* session = dynamic_cast<ActiveSession*>((Session*)socket->getUser());
		if (session) {
			session->setPingTime(25);
			session->setOvertime(30);

			printf("MSGID_PONG!\n");
		}
	}
	return;

	case eMSGID_SessionVerifyRet:
		verifySessionRet(socket, packet);
		return;
	default:
		break;
	}

	Session* session = (Session*)socket->getUser();
	if (!session) return;

	m_handler.dispense(session, packet);
}

void ConnectTask::verifySessionRet(XSocketPtr& socket, NetPacket& packet)
{
	int type;
	int ret;
	packet >> type;
	packet >> ret;
	if (!ret)
	{
		Session* session = NULL;
		if (type == eST_Db) {
			session = SessionMgr::getInstance()->getDb();
		}
		else{
			return;
		}

		session->bindSocket(socket);

		//ping
		session->setOvertime(30);
		session->ping();
		printf("verifySessionRet ping!\n");
	}
}


void ConnectTask::closeSession(XSocketPtr& socket)
{
	Session* session = (Session*)socket->getUser();
	if (session) {
		session->clearSocket();
	}

	//reconnect
	std::shared_ptr<XSocket> dbsocket = std::shared_ptr<XSocket>(new XSocket(socket->getNetThread(), this), [](void*) {printf("~XSocket()"); }); //(std::make_shared<XSocket>(socket->getNetThread(), this));
	dbsocket->connect(socket->GetRemoteIpAddress(), socket->GetRemotePort());
}
