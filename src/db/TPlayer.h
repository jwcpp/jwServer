#pragma once

#include <unordered_map>

#include "../db_share/DBPlayer.h"

#include "ObjectPool.h"
#include "BaseType.h"
#include <memory>

// objectpool
class PlayerSave
{
public:
	DBPlayer m_dbplayer;
	char m_sqloper = 0;
	int8 m_state = 1;//online

	void clear() {
		m_dbplayer.clear();
		m_sqloper = 0;
		m_state = 1;
	}

	DefPoolObj()
};

struct OfflineInfo
{
	OfflineInfo(uint64 pid):
		playerid(pid){}
	uint64 playerid;
	int sec = 60 * 30;
};

class NetPacket;
class SqlStatement;
class ExecCallback;
class Session;
class TPlayer : public ObjectPool<PlayerSave>
{
public:
	TPlayer();
	~TPlayer();

	void updateSec();
	void tick();
	void load(Session * session,uint64_t playerid);
	void createPlayer(uint64_t playerid, std::string && name);
	void saveMsg(uint64_t playerid, NetPacket & packet);
	void offline(uint64_t playerid);
protected:
	void fromDb(Session* session, uint64 playerid);
	void sendGame(Session* session, uint64 playerid, PlayerSave * saveobj, int8 ident);
	void execSql(uint64 playerid, PlayerSave * saveobj);
	void makeField(PlayerSave * saveobj, eDBPlayerAttr attr, std::string & field, SqlStatement * sqlst);
private:
	std::unordered_map<uint64_t, PlayerSave*> m_players;
	std::list<std::shared_ptr<ExecCallback> > m_backcall;
	std::list<OfflineInfo> m_offline;
};

