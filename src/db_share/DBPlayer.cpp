#include "DBPlayer.h"
#include "NetPacket.h"

int g_player_save_time = 60;
DBPlayer::DBPlayer()
{
	clear();
}

void DBPlayer::clear()
{
	m_lv = 0;
	m_exp = 0;
	m_gold = 0;
	m_name = "";
	m_task.clear();

	m_change.clear();
	m_savesec = 0;
}

void DBPlayer::save(eDBPlayerAttr attr)
{
	m_change.insert(attr);
	m_savesec = m_savesec == 0 ? g_player_save_time : m_savesec;
}

bool DBPlayer::checkSec()
{
	if (m_savesec == 0) return false;

	return --m_savesec == 0 ? true : false;
}


void DBPlayer::writeMsg(NetPacket& packet,eDBPlayerAttr attr)
{
	packet << (uint8)attr;
	switch (attr)
	{
	case eDBPA_Lv:
		packet << m_lv;
		break;
	case eDBPA_Exp:
		packet << m_exp;
		break;
	case eDBPA_Gold:
		packet << m_gold;
		break;
	case eDBPA_Name:
		packet << m_name;
		break;
	case eDBPA_Task:
	{
		int len = m_task.size();
		packet << len;
		for (auto it = m_task.begin(); it != m_task.end(); ++it)
		{
			packet << *it;
		}
	}
		break;
	default:
		break;
	}
}

void DBPlayer::readMsg(NetPacket& packet, eDBPlayerAttr attr)
{
	switch (attr)
	{
	case eDBPA_Lv:
		packet >> m_lv;
		break;
	case eDBPA_Exp:
		packet >> m_exp;
		break;
	case eDBPA_Gold:
		packet >> m_gold;
		break;
	case eDBPA_Name:
		packet >> m_name;
		break;
	case eDBPA_Task:
	{
		int len;
		packet >> len;
		m_task.clear();
		for (int i = 0; i< len; ++i)
		{
			int data;
			packet >> data;
			m_task.insert(data);
		}
	}
		break;
	default:
		break;
	}

	save(attr);
}