#include "MySqlCommon.h"
#include "mysql/mysql.h"

namespace MySQL
{
	int threadSafe()
	{
		return mysql_thread_safe();
	}
	void libraryInit()
	{
		mysql_library_init(-1, nullptr, nullptr);
	}
	void libraryEnd()
	{
		mysql_library_end();
	}
	char const* getLibraryVersion()
	{
		return MYSQL_SERVER_VERSION;
	}
};