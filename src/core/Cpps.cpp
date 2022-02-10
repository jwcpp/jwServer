#include "Post.h"

Post::~Post()
{
	while (!mPostlist.empty())
	{
		delete mPostlist.front();
		mPostlist.pop_front();
	}
}

void Post::add(PostBase* node) {
	mLock.lock();
	mPostlist.push_back(node);
	mLock.unlock();
}

void Post::update()
{
	mLock.lock();
	if (!mPostlist.empty())
	{
		for (auto it = mPostlist.begin(); it != mPostlist.end(); ++it) {
			PostBase* node = *it;
			node->call();
			delete node;
		}
		mPostlist.clear();
	}
	mLock.unlock();
}


#include "HttpServer.h"

void HttpServer::add_get(const std::string& path, std::function<void(std::shared_ptr<HttpServer::Response>, std::shared_ptr<HttpServer::Request>)> func)
{
	this->resource[path]["GET"] = [this, func](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
		//��Ҫ��std::move(),���򴫹�ȥ֮������ָ��ͱ��ͷ���
		this->post(func, std::move(response), std::move(request));
	};
}

void HttpServer::add_post(const std::string& path, std::function<void(std::shared_ptr<HttpServer::Response>, std::shared_ptr<HttpServer::Request>)> func)
{
	this->resource[path]["POST"] = [this, func](std::shared_ptr<HttpServer::Response> response, std::shared_ptr<HttpServer::Request> request) {
		this->post(func, std::move(response), std::move(request));
	};
}

#include "HttpClientMgr.h"

void HttpClientMgr::http_get(const char* addr, const std::string& path, std::string_view content, std::function<void(std::shared_ptr<HttpClient::Response>, int)> callback_, const CaseInsensitiveMultimap& header)
{
	//auto client = std::shared_ptr<HttpClient>(new HttpClient(addr), [](void*) {printf("~HttpClient\n"); });
	auto client = std::make_shared<HttpClient>(addr);
	client->io_service = m_io;
	client->request("GET", path, content, header, [this, client, callback_](std::shared_ptr<HttpClient::Response> response, const SimpleWeb::error_code& ec) {
		if (!callback_) return;

		// ����ֱ�ӵ���Post::post(callback_, ....)����Ҫ�ٰ�һ�㣬��ֹclient������ǰ�ͷ�
		int ec_value = ec.value();
		Post::post([client, callback_, response, ec_value]() {
			callback_(response, ec_value);
			});
		});
}

//127.0.0.1:80
void HttpClientMgr::http_post(const char* addr, const std::string& path, std::string_view content, std::function<void(std::shared_ptr<HttpClient::Response>, int)> callback_, const CaseInsensitiveMultimap& header)
{
	auto client = std::make_shared<HttpClient>(addr);
	client->io_service = m_io;
	client->request("POST", path, content, header, [this, client, callback_](std::shared_ptr<HttpClient::Response> response, const SimpleWeb::error_code& ec) {
		if (!callback_) return;

		int ec_value = ec.value();
		Post::post([client, callback_, response, ec_value]() {
			callback_(response, ec_value);
			});
		});
}