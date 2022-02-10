#include "MsgHandler.h"
#include "NetPacket.h"
#include "MsgProtocol.h"
#include "Session.h"
#include "ProtoDef.h"
#include "HttpMsg.h"

template<>
void MsgHandler::process<eMSGID_W2D_Login>(Session * session, NetPacket & packet)
{
	std::string user;
	uint64 account_id;
	packet >> user;
	packet >> account_id;

	printf("login account_id:%lld\n", account_id);

	HttpMsg::getInstance()->loginRet(user, account_id);

	return;
}

void MsgHandler::dispense(Session* session, NetPacket& packet)
{
	SWITCH_MSG_BEGIN(packet.getOpcode())
		// begin
		SELECT_MSG_ID(eMSGID_W2D_Login)

		//end
	SWITCH_MSG_END()
}