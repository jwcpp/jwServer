#include "TableManage.h"
#include "TableParser.h"
#include "Log.h"

#define LOAD_TABLE(obj, name)\
ret = obj->load(path + name + ".txt");\
if(!ret){\
	PRINT_ERROR("load table(%s) error", name);\
	return ret;\
}

#define RELOAD_TABLE(obj, name)\
if (!fileName || strcmp(name, fileName) == 0) {\
	ret = obj->reload(path + name + ".txt");\
	if (!ret || fileName) return ret;\
}



#include "TabTest.h"


TableManage::TableManage()
{

}

bool TableManage::init()
{
	m_TabTest = new TabTest();

	return load();
}

void TableManage::release()
{
	if (m_TabTest) delete m_TabTest;
}

bool TableManage::load()
{
	bool ret = true;
	std::string path = "./"; // config/

	LOAD_TABLE(m_TabTest, "test")

	return ret;
}

bool TableManage::reload(const char* fileName)
{
	bool ret = true;
	std::string path = "./";

	RELOAD_TABLE(m_TabTest, "test")

	return ret;
}