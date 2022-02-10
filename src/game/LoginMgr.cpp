#include "LoginMgr.h"
#include "XSocket.h"
#include "SessionMgr.h"
#include "NetPacket.h"
#include "MsgProtocol.h"

void LoginMgr::w2sCommand(uint64 accid, uint16 comd)
{
	m_loginQueue.emplace_back(accid, comd);
}

void LoginMgr::c2sLogin(XSocketPtr& socket, uint64 accountid, uint16 comd)
{
	auto it = m_loginQueue.begin();
	for (; it != m_loginQueue.end(); ++it)
	{
		if (it->accountid == accountid && it->command == comd)
		{
			break; // ok
		}
	}

	if (it == m_loginQueue.end())
	{
		// send eMSGID_S2C_LoginFail
		return; // fail
	}

	it->socket = std::move(socket);

	// load role
	NetPacket pack(eMSGID_S2D_LoadRole);
	pack << accountid;
	SessionMgr::getInstance()->getDb()->sendPacket(pack);
}

void LoginMgr::updateSec()
{
	for (auto it = m_loginQueue.begin(); it != m_loginQueue.end(); )
	{
		if (--(it->sec) <= 0)
		{
			it = m_loginQueue.erase(it);
		}
		else
		{
			break;
		}
	}
}

bool LoginMgr::loginOk(uint64 accid, XSocketPtr& ptr)
{
	
	for (auto it = m_loginQueue.begin(); it != m_loginQueue.end(); ++it)
	{
		if (it->accountid == accid)
		{
			ptr = std::move(it->socket);
			m_loginQueue.erase(it);
			return true;
		}
	}

	return false;
}

void LoginMgr::disconnect(XSocketPtr& ptr)
{
	for (auto it = m_loginQueue.begin(); it != m_loginQueue.end(); ++it)
	{
		if (it->socket == ptr)
		{
			m_loginQueue.erase(it);
			break;
		}
	}
}