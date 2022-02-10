#pragma once
#include <string>
#include "BaseType.h"
#include <map>

struct ConfigAddr
{
	std::string ip;
	uint16 port;
};

class ServerConfig
{
public:
	int load(const char * filename);

	const char * verifyKey() { return verify_key.c_str(); }

	//webserver
	const ConfigAddr* httpAddr() const { return &m_http; }
	const ConfigAddr* webAddr() const { return &m_web; }

	// db
	const ConfigAddr* dbAddr() const { return &m_db; }
	// database
	const char* dbHost() { return m_dbHost.c_str(); }
	const char* dbUser() { return m_dbUser.c_str(); }
	const char* dbPass() { return m_dbPass.c_str(); }
	const char* database() { return m_database.c_str(); }

	// game
	const ConfigAddr* gameAddr(int id) const {
		auto it = m_game.find(id);
		if (it == m_game.end()) return NULL;
		return &it->second;
	}

	const std::map<int, ConfigAddr>* games() { return &m_game;}
private:
	std::string verify_key;

	//web
	ConfigAddr m_http;
	ConfigAddr m_web;

	//db
	ConfigAddr m_db;
	std::string m_dbHost;
	std::string m_dbUser;
	std::string m_dbPass;
	std::string m_database;

	//game
	std::map<int, ConfigAddr> m_game;
};

