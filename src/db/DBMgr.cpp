#include "DBMgr.h"
#include "database/Database.h"
#include "Util.h"
#include "ServerConfig.h"
#include "TAccount.h"
#include "Log.h"
#include "TPlayer.h"

Database * gReadDatabase = NULL;
Database * gWriteDatabase = NULL;

Database* readDatabase()
{
	return gReadDatabase;
}
Database* writeDatabase()
{
	return gWriteDatabase;
}
Database* randomDatabase()
{
	int ran = Util::random(0, 1);
	if (ran) return readDatabase();
	else return writeDatabase();
}


int DBMgr::init(ServerConfig * config)
{
	gReadDatabase = new Database(config->dbHost(), config->database(), config->dbUser(), config->dbPass());
	gWriteDatabase = new Database(config->dbHost(), config->database(), config->dbUser(), config->dbPass());

	gReadDatabase->create(3);
	gWriteDatabase->create(3);

	if (!gReadDatabase->open()) {
		PRINT_ERROR("connect mysql error!");
		return -1;
	}
	if (!gWriteDatabase->open()){
		PRINT_ERROR("connect mysql error!");
		return -2;
	}

	gReadDatabase->start();
	gWriteDatabase->start();


	mAccount = new TAccount;
	mAccount->load();

	mPlayer = new TPlayer;


	return 0;
}

void DBMgr::stop()
{
	if (gReadDatabase)gReadDatabase->stop();
	if (gWriteDatabase)gWriteDatabase->stop();
}

void DBMgr::release()
{
	if (gReadDatabase) delete gReadDatabase;
	if (gWriteDatabase) delete gWriteDatabase;
	if (mAccount) delete mAccount;
	if (mPlayer) delete mPlayer;
}

void DBMgr::updateSec()
{
	mPlayer->updateSec();
}

void DBMgr::tick()
{
	mPlayer->tick();
}

uint64 DBMgr::login(std::string& user, std::string& pass)
{
	uint64_t accid = 0;
	TAccountinfo * info = mAccount->getInfo(user);
	if (info)
	{
		if (pass == info->pass)
		{
			accid = info->id;
		}
	}
	else
	{
		accid = mAccount->creatAccount(user, pass);
	}

	return accid;
}

void DBMgr::loadPlayer(Session* session, uint64 playerid)
{
	mPlayer->load(session, playerid);
}
void DBMgr::createPlayer(uint64 playerid, std::string&& name)
{
	mPlayer->createPlayer(playerid, std::move(name));
}
void DBMgr::savePlayer(uint64 playerid, NetPacket & pack)
{
	mPlayer->saveMsg(playerid, pack);
}
void DBMgr::offline(uint64 playerid)
{
	mPlayer->offline(playerid);
}