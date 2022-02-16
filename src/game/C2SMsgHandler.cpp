#include "C2SMsgHandler.h"
#include "NetPacket.h"
#include "MsgProtocol.h"
#include "Session.h"
#include "ProtoDef.h"
#include "PlayerMgr.h"
#include "Player.h"

//client msg

template<>
void C2SMsgHandler::process<0>(Session* session, NetPacket& packet)
{
	//uint64 playerid;
	//packet >> playerid;
	//std::string name;
	//packet >> name;

	
}

void C2SMsgHandler::dispense(Session* session, NetPacket& packet)
{
	SWITCH_MSG_BEGIN(packet.getOpcode())
		// begin
		SELECT_MSG_ID(0)

		//end
		SWITCH_MSG_END()
}