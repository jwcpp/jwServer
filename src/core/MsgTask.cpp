#include "MsgTask.h"
#include "XSocket.h"

void MsgTask::onAccept(XSocketPtr&& socket)
{
	_msgqueue.push(std::pair<XSocketPtr, NetPacket>(socket, NetPacket(MSGID_ACCEPT)));
}

void MsgTask::onConnect(XSocketPtr&& socket, const std::error_code& error)
{
	NetPacket packet(MSGID_CONNECT);
	packet << error.value();
	_msgqueue.push(std::pair<XSocketPtr, NetPacket>(socket, std::move(packet)));
}

void MsgTask::onPacket(XSocketPtr&& socket, NetPacket& packet)
{
	_msgqueue.push(std::pair<XSocketPtr, NetPacket>(socket, std::move(packet)));
}

void MsgTask::onClose(XSocketPtr&& socket)
{
	_msgqueue.push(std::pair<XSocketPtr, NetPacket>(socket, NetPacket(MSGID_CLOSE)));
}

void MsgTask::update()
{
	_msgqueue.swap();
	std::pair<XSocketPtr, NetPacket> pair_;
	while (_msgqueue.pop(pair_))
	{
		onMsg(pair_.first, pair_.second.getOpcode(), pair_.second);
	}
}