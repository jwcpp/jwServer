#include "MsgHandler.h"
#include "NetPacket.h"
#include "MsgProtocol.h"
#include "DBMgr.h"
#include "Session.h"
#include "ProtoDef.h"

template<>
void MsgHandler::process<eMSGID_W2D_Login>(Session * session, NetPacket & packet)
{
	std::string username;
	std::string pass;
	packet >> username;
	packet >> pass;

	printf("login: user:%s pass:%s\n", username.c_str(), pass.c_str());

	auto id = DBMgr::getInstance()->login(username, pass);
	NetPacket retPack(eMSGID_W2D_Login);
	retPack << username;
	retPack << id;
	session->sendPacket(retPack);
	return;
}

template<>
void MsgHandler::process<eMSGID_S2D_LoadRole>(Session* session, NetPacket& packet)
{
	uint64 playerid;
	packet >> playerid;
	DBMgr::getInstance()->loadPlayer(session, playerid);
	return;
}

template<>
void MsgHandler::process<eMSGID_S2D_CreatePlayer>(Session* session, NetPacket& packet)
{
	uint64 playerid;
	std::string name;
	packet >> playerid;
	packet >> name;
	DBMgr::getInstance()->createPlayer(playerid, std::move(name));
	return;
}

template<>
void MsgHandler::process<eMSGID_S2D_SavePlayer>(Session* session, NetPacket& packet)
{
	uint64 playerid;
	packet >> playerid;
	DBMgr::getInstance()->savePlayer(playerid, packet);
	return;
}

template<>
void MsgHandler::process<eMSGID_S2D_Offline>(Session* session, NetPacket& packet)
{
	uint64 playerid;
	packet >> playerid;
	DBMgr::getInstance()->offline(playerid);
	return;
}

void MsgHandler::dispense(Session* session, NetPacket& packet)
{
	SWITCH_MSG_BEGIN(packet.getOpcode())
		// begin
		SELECT_MSG_ID(eMSGID_W2D_Login)
		SELECT_MSG_ID(eMSGID_S2D_LoadRole)
		SELECT_MSG_ID(eMSGID_S2D_CreatePlayer)
		SELECT_MSG_ID(eMSGID_S2D_SavePlayer)
		SELECT_MSG_ID(eMSGID_S2D_Offline)

		//end
	SWITCH_MSG_END()
}