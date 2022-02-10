#pragma once

#include "Socket.h"

class NetThread;
class Accept
{
public:

	Accept(std::function<SocketPtr()> createfunc, std::function<bool(SocketPtr)> acceptfunc = nullptr);
	virtual ~Accept();
	void start(const char* ip, unsigned short port, NetThread * net_thread);
	void close();
protected:
	void startAccept();
	void handleAccept(SocketPtr connect, const std::error_code& error);
private:
	asio::ip::tcp::acceptor* _acceptor;
	std::function<SocketPtr()> _createfunc;
	std::function<bool(SocketPtr)> _acceptfunc;
	std::atomic<bool> _closed;
};

