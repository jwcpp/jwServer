#pragma once

#include <map>
#include <string>
#include <set>

enum eDBPlayerAttr
{
	eDBPA_Lv,
	eDBPA_Exp,
	eDBPA_Gold,
	eDBPA_Name,
	eDBPA_Task,
};

extern int g_player_save_time;

class NetPacket;
class DBPlayer
{
public:
	DBPlayer();
	void clear();
	void save(eDBPlayerAttr attr);
	bool checkSec();
	void writeMsg(NetPacket& packet, eDBPlayerAttr attr);
	void readMsg(NetPacket& packet, eDBPlayerAttr attr);
	std::set<eDBPlayerAttr>& change() { return m_change; }

	//attrs
	int m_lv;
	int m_exp;
	int m_gold;
	std::string m_name;
	std::set<int> m_task;

private:
	std::set<eDBPlayerAttr> m_change;
	int m_savesec = 0;
};

