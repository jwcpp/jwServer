#pragma once

#include "TableReader.h"
#include <map>

class TabTest : public TableReader
{
	struct Data
	{
		void operator=(Data&& data){
			num = data.num;
			name = std::move(data.name);
		}

		int num;
		std::string name;
	};

public:
	~TabTest() {}

	Data* getData(int id);

	virtual bool onLoad(TableParser* parser);
	virtual bool onReload(TableParser* parser);
private:
	std::map<int, Data> m_datas;
};

