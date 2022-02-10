#include "PlayerMgr.h"
#include "Player.h"
#include "NetPacket.h"
#include "MsgProtocol.h"
#include "SessionMgr.h"

PlayerMgr::PlayerMgr():m_playerPool(NULL)
{
	m_playerPool = new ObjectPool<Player>();
}

PlayerMgr::~PlayerMgr()
{
	if (m_playerPool) delete m_playerPool;
}

Player* PlayerMgr::load(NetPacket& packet, uint64 playerid)
{
	if (m_players.find(playerid) != m_players.end()) return NULL;

	auto player = m_playerPool->create();
	packet >> player->getDb()->m_lv;
	packet >> player->getDb()->m_exp;
	packet >> player->getDb()->m_gold;
	packet >> player->getDb()->m_name;

	int count;
	packet >> count;

	for (int i = 0; i< count; ++i)
	{
		int id;
		packet >> id;
		player->getDb()->m_task.insert(id);
	}

	m_players.emplace(playerid, player);

	return player;
}

void PlayerMgr::updateSec()
{
	for (auto it = m_players.begin(); it != m_players.end(); )
	{
		// save
		if (it->second->getDb()->checkSec())
		{
			NetPacket packet(eMSGID_S2D_SavePlayer);
			packet << it->first;

			auto set_ = it->second->getDb()->change();
			for (auto it_ = set_.begin(); it_ != set_.end(); ++it_)
			{
				it->second->getDb()->writeMsg(packet, *it_);
			}

			SessionMgr::getInstance()->getDb()->sendPacket(packet);
		}
		// close
		else if (!it->second->updateSecond())
		{
			it = m_players.erase(it);

			NetPacket packet(eMSGID_S2D_Offline);
			packet << it->first;
			SessionMgr::getInstance()->getDb()->sendPacket(packet);
		}
		else
		{
			++it;
		}
	}
}