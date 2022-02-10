#pragma once

#include "NetEvent.h"
#include "NetPacket.h"
#include "MsgQueue.h"

class MsgTask : public NetEvent
{
public:
	virtual void update();
protected:
	virtual void onAccept(XSocketPtr&& socket);
	virtual void onConnect(XSocketPtr&& socket, const std::error_code& error);
	virtual void onPacket(XSocketPtr&& socket, NetPacket& packet);
	virtual void onClose(XSocketPtr&& socket);

	virtual void onMsg(XSocketPtr& socket, MSGID msgid, NetPacket & packet) = 0;

protected:
	MsgQueue<std::pair<XSocketPtr, NetPacket> > _msgqueue;
};

