#include "TPlayer.h"
#include "DbGlobal.h"
#include "Database/Database.h"
#include "Database/SqlStatement.h"
#include "Database/DBTask.h"
#include "Database/SqlResultSet.h"
#include "NetPacket.h"
#include "Session.h"
#include "MsgProtocol.h"

TPlayer::TPlayer()
{

}
TPlayer::~TPlayer()
{
	for (auto it = m_players.begin(); it != m_players.end(); ++it)
	{
		free(it->second);
	}
}

void TPlayer::updateSec()
{
	for (auto it = m_players.begin(); it != m_players.end(); ++it)
	{
		if (it->second->m_dbplayer.checkSec())
		{
			//save to db
			execSql(it->first, it->second);
		}
	}

	for (auto it = m_offline.begin(); it != m_offline.end();)
	{
		if (--it->sec == 0)
		{
			m_offline.erase(it);
			auto fpit = m_players.find(it->playerid);
			if (!fpit->second->m_state)
			{
				free(fpit->second);
				m_players.erase(m_players.find(it->playerid));
			}
		}
		else
		{
			break;
		}
	}
}

void TPlayer::tick()
{
	for (auto it = m_backcall.begin(); it != m_backcall.end(); )
	{
		if ((*it)->complete())
		{
			it = m_backcall.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void TPlayer::load(Session* session, uint64_t playerid)
{
	auto it = m_players.find(playerid);
	if (it == m_players.end())
	{
		//从db中加载
		fromDb(session, playerid);
	}
	else
	{
		sendGame(session, playerid, it->second, 0);
	}
}

void TPlayer::createPlayer(uint64_t playerid, std::string&& name)
{
	auto it = m_players.find(playerid);
	if (it != m_players.end()) {
		return;
	}

	//TODO ... check name

	PlayerSave* player = create();
	player->m_sqloper = 1;
	player->m_dbplayer.m_name = std::move(name);
	m_players[playerid] = player;

	player->m_dbplayer.save(eDBPA_Name);
}

void TPlayer::fromDb(Session* session, uint64 playerid)
{
	auto sqlptr = std::make_shared<SqlStatement>("select * from player where id = ?");
	sqlptr->pushUint64(playerid);
	auto callback = readDatabase()->asyncQuery(sqlptr);
	callback->addCallback([session, playerid, this](uint32 erron_, SqlResultSet* res_) {
		if (erron_)
		{
			sendGame(session, playerid, NULL, 2);
			return;
		}

		if (!res_)
		{
			sendGame(session, playerid, NULL, 1);
			return;
		}

		while (res_->fetch())
		{
			PlayerSave* player = create();
			// res_->getField(0) ==> id
			player->m_dbplayer.m_lv = res_->getField(1)->getValue<int>();
			player->m_dbplayer.m_exp = res_->getField(2)->getValue<int>();
			player->m_dbplayer.m_gold = res_->getField(3)->getValue<int>();
			player->m_dbplayer.m_name = std::move(res_->getField(4)->str());
			
			int len = res_->getField(5)->length();
			if (len)
			{
				ByteBuffer buffer;
				buffer.append((uint8 const*)res_->getField(5)->data(), len);
				int count;
				buffer >> count;
				for (int i = 0; i< count; ++i)
				{
					int taskid;
					buffer >> taskid;
					player->m_dbplayer.m_task.insert(taskid);
				}
			}

			m_players[playerid] = player;
			sendGame(session, playerid, player, 0);
		}
		});
		
	m_backcall.push_back(std::move(callback));
}

void TPlayer::sendGame(Session* session,uint64 playerid, PlayerSave* saveobj, int8 ident)
{
	if(saveobj)saveobj->m_state = 1;

	NetPacket pack(eMSGID_D2S_LoadRole);
	pack << playerid;
	pack << ident;
	pack << saveobj->m_dbplayer.m_lv;
	pack << saveobj->m_dbplayer.m_exp;
	pack << saveobj->m_dbplayer.m_gold;
	pack << saveobj->m_dbplayer.m_name;

	pack << (int)saveobj->m_dbplayer.m_task.size();
	for (auto it = saveobj->m_dbplayer.m_task.begin(); it != saveobj->m_dbplayer.m_task.end(); ++it)
	{
		pack << *it;
	}

	session->sendPacket(pack);
}

void TPlayer::saveMsg(uint64_t playerid, NetPacket& packet)
{
	auto fit = m_players.find(playerid);
	if (fit == m_players.end()) return;

	while (packet.rpos() < packet.size())
	{
		uint8 type;
		packet >> type;
		fit->second->m_dbplayer.readMsg(packet, (eDBPlayerAttr)type);
	}
}

void TPlayer::offline(uint64_t playerid)
{
	m_offline.emplace_back(playerid);
}

void TPlayer::execSql(uint64 playerid, PlayerSave* saveobj)
{
	// insert into test set id= 111,name='jw'
	// update test set name = 'c++' where id = 3;

	std::set<eDBPlayerAttr>& attrs = saveobj->m_dbplayer.change();
	if (attrs.size() == 0) return;

	// sql
	std::string sqlstr;
	auto sqlptr = std::make_shared<SqlStatement>(attrs.size() + 1); // id
	if (saveobj->m_sqloper)
	{
		sqlstr = "insert into player set id=? ";
		sqlptr->pushUint64(playerid);

		for (auto it = attrs.begin(); it != attrs.end(); ++it)
		{
			std::string field;
			makeField(saveobj, *it, field, sqlptr.get());
			sqlstr = sqlstr + "," + field + " =? ";
		}
	}
	else
	{
		sqlstr = "update player set";
		bool isfirst = true;
		for (auto it = attrs.begin(); it != attrs.end(); ++it)
		{
			std::string field;
			makeField(saveobj, *it, field, sqlptr.get());
			if (isfirst)
			{
				isfirst = false;
				sqlstr = sqlstr + " " + field + " =? ";
			}
			else
			{
				sqlstr = sqlstr + "," + field + " =? ";
			}
		}

		sqlstr += "where id=?";
		sqlptr->pushUint64(playerid);
	}

	std::set<eDBPlayerAttr> newset = saveobj->m_dbplayer.change();
	saveobj->m_dbplayer.change().clear();

	sqlptr->setSql(std::move(sqlstr));
	auto callback = writeDatabase()->asyncExec(sqlptr);
	callback->addCallback([this, playerid, newset](uint32 erron_, SqlResultSet* res_) {
		auto fit = m_players.find(playerid);
		if (fit == m_players.end()) return;
		if (erron_) {
			// fail, 重新保存
			for (auto it = newset.begin(); it != newset.end(); ++it){
				fit->second->m_dbplayer.save(*it);
			}
		}
		else {
			// ok
			if (fit->second->m_sqloper) { // insert ok
				fit->second->m_sqloper = 0;
			}
		}
		});
	m_backcall.push_back(std::move(callback));
}

void TPlayer::makeField(PlayerSave* saveobj, eDBPlayerAttr attr, std::string& field, SqlStatement* sqlst)
{
	switch (attr)
	{
	case eDBPA_Lv:
		field = "lv";
		sqlst->pushInt32(saveobj->m_dbplayer.m_lv);
		break;
	case eDBPA_Exp:
		field = "exp";
		sqlst->pushInt32(saveobj->m_dbplayer.m_exp);
		break;
	case eDBPA_Gold:
		field = "gold";
		sqlst->pushInt32(saveobj->m_dbplayer.m_gold);
		break;
	case eDBPA_Name:
		field = "name";
		sqlst->pushString(saveobj->m_dbplayer.m_name);
		break;
	case eDBPA_Task:
	{
		field = "task";
		ByteBuffer buffer;
		buffer << (int)saveobj->m_dbplayer.m_task.size();
		for (auto it = saveobj->m_dbplayer.m_task.begin(); it != saveobj->m_dbplayer.m_task.end(); ++it)
		{
			buffer << *it;
		}
		std::string_view strv((const char *)buffer.contents(), buffer.wpos());
		sqlst->pushData(strv);
	}
	break;
	default: break;
	}
}