#pragma once
#include "Session.h"
#include "Singleton.h"
#include <map>

// gameserver
class GameSession : public ActiveSession
{
public:
	void setPlayerCount(int count) { playerCount = count; }
	int getPlayerCount() { return playerCount; }
private:
	int playerCount = 0;
};

class SessionMgr : public Util::Singleton<SessionMgr>
{
public:
	Session* getDb() { return &m_db; }
	void updateSec();
private:
	ActiveSession m_db;
	std::map<int, GameSession> m_games;
};

