#include "Session.h"
#include "XSocket.h"
#include "NetPacket.h"

Session::Session():
	m_overtime(0)
{

}

Session::~Session()
{

}

void Session::clearSocket()
{
	if (m_ptrSocket) {
		m_ptrSocket->setUser(NULL);
		m_ptrSocket = nullptr;
	}

	m_overtime = 0;
}

void Session::bindSocket(XSocketPtr socket_)
{
	if (socket_)
	{
		m_ptrSocket = std::move(socket_);
		m_ptrSocket->setUser(this);
	}
}

void Session::sendPacket(NetPacket& packet)
{
	if(m_ptrSocket) m_ptrSocket->sendPacket(packet);
}
void Session::sendPacket(NetPacket&& packet)
{
	if (m_ptrSocket) m_ptrSocket->sendPacket(std::move(packet));
}

void Session::ping()
{
	NetPacket pack(MSGID_PING);
	sendPacket(pack);
}
void Session::pong()
{
	NetPacket pack(MSGID_PONG);
	sendPacket(pack);
}

bool Session::updateSec()
{
	if (m_overtime)
	{
		if (--m_overtime == 0)
		{
			close();
			return false;
		}
	}

	return true;
}

void Session::close()
{
	if (m_ptrSocket) {
		m_ptrSocket->close();
	}

	clearSocket();
}


ActiveSession::ActiveSession() :
	m_pingTime(0)
{
}

void ActiveSession::clearSocket()
{
	Session::clearSocket();
	m_pingTime = 0;
}

bool ActiveSession::updateSec()
{
	bool ret = Session::updateSec();

	if (m_pingTime)
	{
		if (--m_pingTime == 0)
		{
			ping();
		}
	}

	return ret;
}