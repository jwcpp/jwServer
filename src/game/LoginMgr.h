#pragma once

#include "Singleton.h"
#include "NetDef.h"
#include "BaseType.h"
#include <list>

struct LoginInfo
{
	LoginInfo(uint64 accid, uint16 comd):accountid(accid), command(comd){
	
	}
	uint64 accountid;
	uint16 command;
	XSocketPtr socket;
	int sec = 60;
};

class LoginMgr : public Util::Singleton<LoginMgr>
{
public:
	void w2sCommand(uint64 accid, uint16 comd);
	void c2sLogin(XSocketPtr & socket, uint64 accountid, uint16 comd);
	void updateSec();
	bool loginOk(uint64 accid, XSocketPtr & ptr);
	void disconnect(XSocketPtr& ptr);
private:
	std::list<LoginInfo> m_loginQueue;
};

