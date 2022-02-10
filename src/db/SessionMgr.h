#pragma once
#include "Session.h"
#include "Singleton.h"
#include <map>

class SessionMgr : public Util::Singleton<SessionMgr>
{
public:
	template<typename MAP>
	void init(const MAP* games){
		for (auto it = games->begin(); it != games->end(); ++it){
			m_games[it->first];
		}
	}

	Session* getWeb() { return &m_web; }
	Session* getGame(int serid);

	void updateSec();
private:
	ActiveSession m_web;
	std::map<int, ActiveSession> m_games;
};

