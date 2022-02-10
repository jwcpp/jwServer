#pragma once

#include <system_error>
#include "NetDef.h"

class NetPacket;

class NetEvent
{
public:
	virtual ~NetEvent() {}

	virtual void onAccept(XSocketPtr&& socket) = 0;
	virtual void onConnect(XSocketPtr&& socket, const std::error_code& error) = 0;
	virtual void onPacket(XSocketPtr&& socket, NetPacket & packet) = 0;
	virtual void onClose(XSocketPtr&& socket) = 0;
};