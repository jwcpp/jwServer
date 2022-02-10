#pragma once
#include "BaseType.h"

class TableManage;
class PlayerMgr;
class LoginMgr;
class StartMgr
{
public:
	int init();
	void stop();
	void release();

	void onUpdate(uint32 diff);
	void onTick(uint32 diff);
	void onSecond();
	void onMinute();
	void onHour();
	void onDay();

private:
	TableManage* m_tabMgr = NULL;
	PlayerMgr* m_playerMgr = NULL;
	LoginMgr* m_loginMgr = NULL;
};

