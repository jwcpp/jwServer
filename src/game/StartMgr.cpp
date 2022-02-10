#include "StartMgr.h"
#include "TableManage.h"
#include "PlayerMgr.h"
#include "LoginMgr.h"

int StartMgr::init()
{
	m_tabMgr = new TableManage;
	if (!m_tabMgr->init()) return -1;

	m_playerMgr = new PlayerMgr;
	m_loginMgr = new LoginMgr;

	return 0;
}

void StartMgr::stop()
{

}
void StartMgr::release()
{
	if (m_tabMgr) {
		m_tabMgr->release();
		delete m_tabMgr;
	}

	if (m_playerMgr) delete m_playerMgr;
	if (m_loginMgr) delete m_loginMgr;
}

void StartMgr::onUpdate(uint32 diff)
{

}
void StartMgr::onTick(uint32 diff)
{

}
void StartMgr::onSecond()
{
	m_playerMgr->updateSec();
}
void StartMgr::onMinute()
{

}
void StartMgr::onHour()
{

}
void StartMgr::onDay()
{

}