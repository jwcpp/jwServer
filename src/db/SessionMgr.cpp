#include "SessionMgr.h"

Session* SessionMgr::getGame(int serid)
{
	auto it = m_games.find(serid);
	if (it == m_games.end()) return NULL;

	return &(it->second);
}

void SessionMgr::updateSec()
{
	if (!m_web.updateSec())
	{
		//overtime
	}
}