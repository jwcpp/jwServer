#pragma once

#include "ObjectPool.h"
#include "Session.h"
#include "../db_share/DBPlayer.h"

class Player : public ActiveSession
{
		DefPoolObj()
public:
	void clear();
	DBPlayer* getDb() { return &m_dbplayer; }
	bool updateSecond();
	void disconnect();
	void connect();
private:
	DBPlayer m_dbplayer;
	int closeSec;
};

