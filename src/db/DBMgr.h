#pragma once

#include "Singleton.h"
#include "BaseType.h"
#include <string>

class ServerConfig;
class TAccount;
class TPlayer;
class Session;
class NetPacket;
class DBMgr: public Util::Singleton<DBMgr>
{
public:
	int init(ServerConfig * config);
	void stop();
	void release();
	void updateSec();
	void tick();

	//login
	uint64 login(std::string &user, std::string & pass);
	void loadPlayer(Session * session, uint64 playerid);
	void createPlayer(uint64 playerid, std::string && name);
	void savePlayer(uint64 playerid, NetPacket & pack);
	void offline(uint64 playerid);
private:
	TAccount* mAccount = NULL;
	TPlayer* mPlayer = NULL;
};

