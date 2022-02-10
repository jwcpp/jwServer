#include "XSocket.h"
#include "NetPacket.h"
#include "NetEvent.h"
#include "NetThread.h"

#define SEND_BUFFER_SIZE 4096

XSocket::XSocket(NetThread* nt, NetEvent* event):
	Socket(nt),
	_event(event),
	_headBuffer(PACKET_HEAD_SIZE)
{

}

void XSocket::sendPacket(NetPacket& packet)
{
	_bufferQueue.enqueue(std::move(packet));
}

void XSocket::sendPacket(NetPacket&& packet)
{
	_bufferQueue.enqueue(std::move(packet));
}

void XSocket::onMessage(MessageBuffer& buffer)
{
	while (buffer.GetActiveSize())
	{
		if (_headBuffer.GetRemainingSpace())
		{
			size_t realhead = std::min(_headBuffer.GetRemainingSpace(), buffer.GetActiveSize());
			_headBuffer.Write(buffer.GetReadPointer(), realhead);
			buffer.ReadCompleted(realhead);

			//head complete
			if (_headBuffer.GetRemainingSpace() == 0)
			{
				_bodyBuffer.Resize(getBodySize());
				goto read_msg_body;//Only head, no body
			}
		}
		else
		{
			read_msg_body:
			if (_bodyBuffer.GetRemainingSpace())
			{
				size_t realbody = std::min(_bodyBuffer.GetRemainingSpace(), buffer.GetActiveSize());
				_bodyBuffer.Write(buffer.GetReadPointer(), realbody);
				buffer.ReadCompleted(realbody);
			}

			//body complete
			if (_bodyBuffer.GetRemainingSpace() == 0)
			{
				NetPacket packet(std::move(_bodyBuffer));
				packet.setOpcode((MSGID)getMsgid());
				onPacket(packet);
				_headBuffer.Reset();
			}
		}
	}
}

void XSocket::onPacket(NetPacket& packet)
{
	//std::dynamic_pointer_cast<XSocket>(shared_from_this());
	_event->onPacket(std::dynamic_pointer_cast<XSocket>(shared_from_this()), packet);
}

void XSocket::onAccept()
{
	_event->onAccept(std::dynamic_pointer_cast<XSocket>(shared_from_this()));
}

void XSocket::onConnect(const std::error_code& error)
{
	_event->onConnect(std::dynamic_pointer_cast<XSocket>(shared_from_this()), error);
}

void XSocket::onClose()
{
	_event->onClose(std::dynamic_pointer_cast<XSocket>(shared_from_this()));
}

void XSocket::writePacketToBuffer(NetPacket& packet, MessageBuffer& buffer)
{
	MSGID msgid = packet.getOpcode();
	int32 len = (int32)packet.getSize();
	buffer.Write(&msgid, sizeof(msgid));
	buffer.Write(&len, sizeof(len));
	if (len > 0) {
		buffer.Write(packet.contents(), packet.getSize());
	}
}

bool XSocket::makeSendBuffer(MessageBuffer& buffer)
{
	NetPacket * peek = _bufferQueue.peek();
	if (!peek) return false;

	// init buffer
	if (peek->getSumSize() > SEND_BUFFER_SIZE)
	{
		buffer = MessageBuffer(peek->getSumSize());
	}
	else if (buffer.GetCapacity() != SEND_BUFFER_SIZE)
	{
		buffer = MessageBuffer(SEND_BUFFER_SIZE);
	}
	else
	{
		buffer.Reset();
	}
	
	// memcpy packet
	while (peek)
	{
		if (buffer.GetRemainingSpace() < peek->getSumSize())
		{
			break;
		}
		writePacketToBuffer(*peek, buffer);
		_bufferQueue.pop();
		peek = _bufferQueue.peek();
	}

	return buffer.GetActiveSize() > 0;
}

int32 XSocket::getMsgid()
{
	MSGID msgid;
	memcpy(&msgid, _headBuffer.GetBasePointer(), sizeof(msgid));
	return msgid;
}

int32 XSocket::getBodySize()
{
	int32 size = 0;
	memcpy(&size, _headBuffer.GetBasePointer() + sizeof(MSGID), sizeof(size));
	return size;
}

