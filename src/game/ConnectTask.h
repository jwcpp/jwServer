#pragma once

#include "MsgTask.h"
#include "MsgHandler.h"

class Session;
class ConnectTask : public MsgTask
{
public:
	ConnectTask(int serverid, const char* key);
	void onMsg(XSocketPtr& socket, MSGID msgid, NetPacket& packet);
	void verifySessionRet(XSocketPtr& socket, NetPacket& packet);
	void closeSession(XSocketPtr& socket);

	void setVerifyKey(const char* key) { m_verify_key = key; }
private:
	int m_serverid;
	std::string m_verify_key;
	MsgHandler m_handler;
};

