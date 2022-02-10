#pragma once

#include "MsgTask.h"

class C2ServerTask : public MsgTask
{
public:
	void onMsg(XSocketPtr& socket, MSGID msgid, NetPacket& packet);

private:
	
};

