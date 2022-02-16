#pragma once

#include "MsgTask.h"
#include "C2SMsgHandler.h"

class C2ServerTask : public MsgTask
{
public:
	void onMsg(XSocketPtr& socket, MSGID msgid, NetPacket& packet);

private:
	C2SMsgHandler c2sHandler;
};

