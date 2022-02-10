#pragma once

#include "Post.h"
#include "Singleton.h"
#define USE_STANDALONE_ASIO 1

using namespace SimpleWeb;

#ifdef USE_HTTPS

#include "http/client_https.hpp"
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTPS>;

#else

#include "http/client_http.hpp"
using HttpClient = SimpleWeb::Client<SimpleWeb::HTTP>;

#endif


class HttpClientMgr : public Post, public Util::Singleton<HttpClientMgr>
{
public:
	//HttpClientMgr();
	HttpClientMgr(asio::io_context * io_):
		m_io(std::shared_ptr<asio::io_context>(io_, [](void*) {})){

	}
	HttpClientMgr(std::shared_ptr<asio::io_context>& io_) :
		m_io(io_) {
	}

	//127.0.0.1:80
	void http_get(const char* addr, const std::string& path, std::string_view content = "", std::function<void(std::shared_ptr<HttpClient::Response>, int)> callback_ = nullptr, const CaseInsensitiveMultimap& header = CaseInsensitiveMultimap());
	//127.0.0.1:80
	void http_post(const char* addr, const std::string& path, std::string_view content = "", std::function<void(std::shared_ptr<HttpClient::Response>, int)> callback_ = nullptr, const CaseInsensitiveMultimap& header = CaseInsensitiveMultimap());
	
private:
	std::shared_ptr<asio::io_context> m_io = NULL;
};