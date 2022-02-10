#include "TabTest.h"
#include "TableParser.h"

TabTest::Data* TabTest::getData(int id)
{
	auto it = m_datas.find(id);
	if (it == m_datas.end()) {
		return NULL;
	}

	return &it->second;
}

bool TabTest::onLoad(TableParser* parser)
{
	parser->getLine();

	TableRow row;
	while (parser->getLine(&row))
	{
		int id = row.get<int>("id");
		Data data;
		data.num = row.get<int>("num");
		data.name = row.moveField("name");
		m_datas[id] = std::move(data);
	}

	return true;
}

bool TabTest::onReload(TableParser* parser)
{
	m_datas.clear();
	return onLoad(parser);
}