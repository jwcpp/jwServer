#pragma once

#include "Socket.h"
#include "readerwriterqueue.h"
class NetPacket;
class NetEvent;
class XSocket : public Socket
{
public:
	XSocket(NetThread * nt, NetEvent * event);

	void sendPacket(NetPacket& packet);
	void sendPacket(NetPacket&& packet);
	void setUser(void* user) { _user = user; }
	void* getUser() { return _user; }
protected:
	virtual void onMessage(MessageBuffer& buffer);
	virtual void onPacket(NetPacket& packet);
	virtual void onAccept();
	virtual void onConnect(const std::error_code& error);
	virtual void onClose();
	void writePacketToBuffer(NetPacket & packet, MessageBuffer & buffer);
	virtual bool makeSendBuffer(MessageBuffer & buffer);

	int32 getMsgid();
	int32 getBodySize();

private:
	NetEvent * _event;
	MessageBuffer _headBuffer;
	MessageBuffer _bodyBuffer;
	moodycamel::ReaderWriterQueue<NetPacket> _bufferQueue;

	void* _user;
};

