#pragma once

#include "NetDef.h"
#include "BaseType.h"
#include "Singleton.h"

class NetPacket;
class Session : public Util::NonCopyable
{
public:
	Session();
	virtual ~Session();

	void ping();
	void pong();

	bool operator == (XSocketPtr& ptr) {
		return m_ptrSocket && m_ptrSocket == ptr;
	}

	virtual void clearSocket();
	void bindSocket(XSocketPtr socket_);
	void sendPacket(NetPacket& packet);
	void sendPacket(NetPacket&& packet);
	virtual bool updateSec();
	bool connectOk() { return m_ptrSocket != nullptr; }
	void close();
	XSocketPtr getSocket() { return m_ptrSocket; }

	void setOvertime(int overtime) { m_overtime = overtime; }

protected:
	XSocketPtr m_ptrSocket;
	int32 m_overtime;
};

class ActiveSession : public Session
{
public:
	ActiveSession();

	virtual void clearSocket();
	void setPingTime(int32 pingTime) { m_pingTime = pingTime; }
	virtual bool updateSec();

protected:
	int32 m_pingTime;
};