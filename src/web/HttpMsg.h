#pragma once
#include "HttpServer.h"
#include "Singleton.h"

struct AccountInfo {

	void operator = (AccountInfo&& info) {
		time = info.time;
		response = std::move(info.response);
	}

	std::shared_ptr<HttpServer::Response> response;
	int time = 20;
};

class HttpMsg:public Util::Singleton<HttpMsg>
{
public:
	void registerHttp(HttpServer* server);

	void updateSec();
	void loginRet(std::string& username, uint64& accountid);
protected:
	void login(std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request);
private:
	std::map<std::string, AccountInfo > accounts;
};

