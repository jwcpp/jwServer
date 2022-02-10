#pragma once

#include "Singleton.h"
#include "ObjectPool.h"
#include <map>
#include "BaseType.h"

class Player;
class NetPacket;
class PlayerMgr: public Util::Singleton<PlayerMgr>
{
public:
	PlayerMgr();
	~PlayerMgr();
	Player* load(NetPacket& packet, uint64 playerid);
	void updateSec();
protected:
	ObjectPool<Player> * m_playerPool;
	std::map<uint64, Player*> m_players;
};

