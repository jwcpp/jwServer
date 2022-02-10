#include "ServerConfig.h"

#define RAPIDXML_STATIC_POOL_SIZE 1

#include "rapidxml-1.13/rapidxml.hpp"
#include "rapidxml-1.13/rapidxml_print.hpp"
#include "rapidxml-1.13/rapidxml_utils.hpp"
#include "Log.h"

using namespace rapidxml;

int ServerConfig::load(const char* filename)
{
	try
	{
		file<> fdoc(filename);
		xml_document<> doc;
		doc.parse<0>(fdoc.data());

		xml_node<>* root = doc.first_node();
		if (!root) return 1;

		xml_node<>* key = root->first_node("verify_key");
		if (!key) return 2;
		verify_key = key->value();

		// webserver
		{
			xml_node<>* webnode = root->first_node("web_server");
			if (!webnode) return 5;

			xml_node<>* ipnode = webnode->first_node("outer_addr");
			xml_node<>* portnode = webnode->first_node("outer_port");
			if (!ipnode || !portnode) return 6;

			m_http.ip = ipnode->value();
			m_http.port = atoi(portnode->value());

			ipnode = webnode->first_node("inter_addr");
			portnode = webnode->first_node("inter_port");
			if (!ipnode || !portnode) return 7;

			m_web.ip = ipnode->value();
			m_web.port = atoi(portnode->value());
		}

		// db
		{
			xml_node<>* dbnode = root->first_node("db_server");
			if (!dbnode) return 10;

			xml_node<>* addr = dbnode->first_node("addr");
			xml_node<>* port = dbnode->first_node("port");
			if (!addr || !port) return 11;

			m_db.ip = addr->value();
			m_db.port = atoi(port->value());

			xml_node<>* db_host = dbnode->first_node("db_host");
			xml_node<>* db_user = dbnode->first_node("db_user");
			xml_node<>* db_pass = dbnode->first_node("db_pass");
			xml_node<>* db_name = dbnode->first_node("database");

			if (!db_host || !db_user || !db_pass || !db_name) return 12;

			m_dbHost = db_host->value();
			m_dbUser = db_user->value();
			m_dbPass = db_pass->value();
			m_database = db_name->value();

		}

		// game
		{
			xml_node<>* game = root->first_node("game_server");
			if (!game) return 20;

			for (auto curr = game->first_node(); curr; curr = curr->next_sibling())
			{
				if (std::string("node") != curr->name())
				{
					return 21;
				}

				int id = atoi(curr->first_attribute("id")->value());

				auto ipnode = curr->first_node("outer_addr");
				auto portnode = curr->first_node("outer_port");
				if (!ipnode || !portnode) return 22;

				ConfigAddr addr;
				addr.ip = ipnode->value();
				addr.port = atoi(portnode->value());
				m_game[id] = addr;
			}
		}
	}
	catch (std::exception& e)
	{
		PRINT_ERROR("load %s, %s", filename, e.what());
		return -1;
	}

	return 0;
}