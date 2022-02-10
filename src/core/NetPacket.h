#pragma once

#include "ByteBuffer.h"

#define PACKET_HEAD_SIZE (2 + 4)

#define UNKNOWN_OPCODE 0

typedef uint16 MSGID;

class NetPacket : public ByteBuffer
{
public:

	NetPacket() : ByteBuffer(0, Reserve{}),m_opcode(UNKNOWN_OPCODE)
	{
	}

	NetPacket(uint32 opcode) : ByteBuffer(0, Reserve{}), m_opcode(opcode){ }

	NetPacket(uint32 opcode, size_t res, Reserve) : ByteBuffer(res, Reserve{}), m_opcode(opcode){ }

	NetPacket(uint32 opcode, size_t res, Resize) : ByteBuffer(res, Resize{}), m_opcode(opcode){ }

	NetPacket(uint32 opcode, size_t res) : NetPacket(opcode, res, Reserve{}) { }

	NetPacket(NetPacket&& packet) noexcept : ByteBuffer(std::move(packet)), m_opcode(packet.m_opcode)
	{
	}

	NetPacket(NetPacket const& right) = default;

	NetPacket& operator=(NetPacket const& right)
	{
		if (this != &right)
		{
			m_opcode = right.m_opcode;
			ByteBuffer::operator =(right);
		}

		return *this;
	}

	NetPacket& operator=(NetPacket&& right) noexcept
	{
		if (this != &right)
		{
			m_opcode = right.m_opcode;
			ByteBuffer::operator=(std::move(right));
		}

		return *this;
	}

	NetPacket(MessageBuffer && buffer) : ByteBuffer(std::move(buffer)), m_opcode(UNKNOWN_OPCODE){ }

	MSGID getOpcode() { return m_opcode; }
	void setOpcode(MSGID opcode) { m_opcode = opcode; }
	size_t getSize() { return wpos(); }
	size_t getSumSize() { return wpos() + PACKET_HEAD_SIZE;}
private:
	MSGID m_opcode = 0;
};

