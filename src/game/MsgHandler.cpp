#include "MsgHandler.h"
#include "NetPacket.h"
#include "MsgProtocol.h"
#include "Session.h"
#include "ProtoDef.h"
#include "PlayerMgr.h"
#include "Player.h"
#include "LoginMgr.h"

template<>
void MsgHandler::process<eMSGID_D2S_LoadRole>(Session* session, NetPacket& packet)
{
	uint64 playerid;
	packet >> playerid;
	int8 ident;
	packet >> ident;

	if (ident == 0)
	{
		Player * player = PlayerMgr::getInstance()->load(packet, playerid);
		if (!player) {
			return;
		}

		//--------------test
		//player->getDb()->m_exp += 1;
		//player->getDb()->m_task.insert(10);
		//player->getDb()->m_task.insert(11);
		//player->getDb()->save(eDBPA_Exp);
		//player->getDb()->save(eDBPA_Task);
		//----------------------

		XSocketPtr socketptr;
		if (!LoginMgr::getInstance()->loginOk(playerid, socketptr))
		{
			return;
		}

		player->bindSocket(socketptr);
		player->setOvertime(35);

		// notice client, player info
	}
	else if (ident == 1)
	{
		// 1. notice client, new create player
		// 2. clinet input player name
	}
	else
	{
		// notice client, load player error
	}

	return;
}

void MsgHandler::dispense(Session* session, NetPacket& packet)
{
	SWITCH_MSG_BEGIN(packet.getOpcode())
		// begin
		SELECT_MSG_ID(eMSGID_D2S_LoadRole)

		//end
	SWITCH_MSG_END()
}