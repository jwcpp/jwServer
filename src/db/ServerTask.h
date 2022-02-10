#pragma once

#include "MsgTask.h"
#include "MsgHandler.h"

class ServerTask : public MsgTask
{
public:
	ServerTask(const char* key);

	void onMsg(XSocketPtr& socket, MSGID msgid, NetPacket& packet);
	void verifySession(XSocketPtr& socket, NetPacket& packet);
	void closeSession(XSocketPtr& socket);

	void setVerifyKey(const char* key) { m_verify_key = key; }
private:
	std::string m_verify_key;
	MsgHandler m_handler;
};

