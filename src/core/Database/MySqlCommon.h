#pragma once

#include <string>

struct SqlConfig
{
	std::string m_ip;
	unsigned int m_port = 3306;
	std::string m_dbname;
	std::string m_user;
	std::string m_pswd;
};

namespace MySQL
{
	int threadSafe();
	void libraryInit();
	void libraryEnd();
	char const* getLibraryVersion();
};

#define Mysql_Concurrent_Queue 1