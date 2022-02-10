#pragma once
#include "Session.h"
#include "Singleton.h"
#include <map>

class SessionMgr : public Util::Singleton<SessionMgr>
{
public:
	Session* getDb() { return &m_db; }
	void updateSec();
private:
	ActiveSession m_db;
};

