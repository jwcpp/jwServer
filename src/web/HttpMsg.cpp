#include "HttpMsg.h"
#include "HttpParam.h"
#include "SessionMgr.h"
#include "NetPacket.h"
#include "MsgProtocol.h"

void HttpMsg::registerHttp(HttpServer* server)
{
	server->add_get("/test", std::bind(&HttpMsg::login, this, std::placeholders::_1, std::placeholders::_2));
}

void HttpMsg::updateSec()
{
	for (auto it = accounts.begin(); it != accounts.end(); )
	{
		if (--(it->second.time) == 0)
		{
			it->second.response->write("http overtime");
			it = accounts.erase(it);
		}
		else
		{
			++it;
		}
	}
}

void HttpMsg::login(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request)
{
	HttpParam httpParam(request->query_string);
	if (httpParam.find("name") && httpParam.find("pass"))
	{
		std::string_view sv_name = httpParam.getStr("name");
		std::string str_name(sv_name.data(), sv_name.size());
		std::string_view sv_pass = httpParam.getStr("pass");

		// send to db
		NetPacket pack(eMSGID_W2D_Login);
		pack << str_name;
		pack << std::string(sv_pass.data(), sv_pass.size());
		SessionMgr::getInstance()->getDb()->sendPacket(pack);

		AccountInfo info;
		info.response = std::move(response);
		accounts[std::move(str_name)] = std::move(info);
	}
}

void HttpMsg::loginRet(std::string& username, uint64& accountid)
{
	auto it = accounts.find(username);
	if (it == accounts.end()) return;

	if (accountid) {
		it->second.response->write("login success");
	}
	else{
		it->second.response->write("login fail");
	}

	accounts.erase(it);
}