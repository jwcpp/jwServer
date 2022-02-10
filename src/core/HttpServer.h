#pragma once

#define USE_STANDALONE_ASIO 1

#include "http/server_http.hpp"
#include "Post.h"
#include "BaseType.h"

class HttpServer : public SimpleWeb::Server<SimpleWeb::HTTP>, public Post
{
public:
	HttpServer(asio::io_context* io)
	{
		this->io_service = std::shared_ptr<asio::io_context>(io, [](void*) {});
	}

	HttpServer(std::shared_ptr<asio::io_context> & io)
	{
		this->io_service = io;
	}

	void run(const char * ip, uint16 port = 80)
	{
		this->config.address = ip;
		this->config.port = port;
		this->start();
	}

	//stop()
	
	void add_get(const std::string& path, std::function<void(std::shared_ptr<HttpServer::Response>, std::shared_ptr<HttpServer::Request>)> func);
	void add_post(const std::string& path, std::function<void(std::shared_ptr<HttpServer::Response>, std::shared_ptr<HttpServer::Request>)> func);
};
