#pragma once

#define USE_STANDALONE_ASIO 1


#include "Post.h"
#include "BaseType.h"

#ifdef USE_HTTPS
	#include "http/server_https.hpp"
	class HttpServer : public Post, public SimpleWeb::Server<SimpleWeb::HTTPS>
	{
		public:
			HttpServer(asio::io_context* io, const std::string& certification_file, const std::string& private_key_file) :
				SimpleWeb::Server<SimpleWeb::HTTPS>(certification_file, private_key_file)
			{
				this->io_service = std::shared_ptr<asio::io_context>(io, [](void*) {});
			}

			HttpServer(std::shared_ptr<asio::io_context> &io, const std::string& certification_file, const std::string& private_key_file):
				SimpleWeb::Server<SimpleWeb::HTTPS>(certification_file, private_key_file)
			{
				this->io_service = io;
			}
#else
	#include "http/server_http.hpp"
	class HttpServer : public Post, public SimpleWeb::Server<SimpleWeb::HTTP>
	{
	public:
		HttpServer(asio::io_context* io)
		{
			this->io_service = std::shared_ptr<asio::io_context>(io, [](void*) {});
		}

		HttpServer(std::shared_ptr<asio::io_context>& io)
		{
			this->io_service = io;
		}
#endif

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
