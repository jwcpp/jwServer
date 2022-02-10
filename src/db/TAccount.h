#pragma once

#include <unordered_map>

struct TAccountinfo
{
	uint64_t id;
	std::string pass;
};

class TAccount
{
public:
	void load();
	TAccountinfo* getInfo(std::string & user);
	uint64_t creatAccount(std::string& user, std::string& pass);
private:
	std::unordered_map<std::string, TAccountinfo> m_datas;
	uint64_t m_incId = 0;
};

