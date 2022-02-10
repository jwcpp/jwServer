#include "C2ServerTask.h"
#include "XSocket.h"
#include "MsgProtocol.h"
#include "Session.h"
#include "LoginMgr.h"
#include "Player.h"

void C2ServerTask::onMsg(XSocketPtr& socket, MSGID msgid, NetPacket& packet)
{
	/*
	#define MSGID_ACCEPT 10
	#define MSGID_CONNECT 11
	#define MSGID_CLOSE 12
	*/

	switch (msgid)
	{
	case MSGID_ACCEPT: return; //if getPlayerCount() > x : closeSocket()
	case MSGID_CLOSE:
	{
		Player* player = dynamic_cast<Player*>((Session*)socket->getUser());
		if (!player) {
			LoginMgr::getInstance()->disconnect(socket);
		}
		else {
			player->clearSocket();
			player->disconnect();
		}
		return;
	}
	case MSGID_PING:
	{
		Session* session = (Session*)socket->getUser();
		if (session) {
			session->setOvertime(35);
			session->pong();
		}

		return;
	}
	
	case eMSGID_C2S_Login:
	{
		uint64 accountid;
		uint16 comd;
		packet >> accountid;
		packet >> comd;
		LoginMgr::getInstance()->c2sLogin(socket, accountid, comd);

		return;
	}
	default:break;
	}
}
