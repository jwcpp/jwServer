#pragma once

#include "Singleton.h"

class TabTest;
class TableManage : public Util::Singleton<TableManage>
{
public:
	TableManage();

	bool init();
	void release();

	bool load();
	bool reload(const char* fileName = NULL);

	TabTest* getTabTest() { return m_TabTest; }

private:
	TabTest* m_TabTest = NULL;
};

