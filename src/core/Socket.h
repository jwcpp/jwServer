#pragma once

#include "asio.hpp"
#include "MessageBuffer.h"
#include <queue>

class NetThread;
class Socket : public std::enable_shared_from_this<Socket>
{
public:
	Socket(NetThread * socketThread);
	virtual ~Socket();

	void start();
	void connect(asio::ip::address addr, uint16 port);
	void connect(const char* ip, uint16 port);
	void close();
	void waitClose();
	virtual bool update();

	asio::ip::tcp::socket& socket() { return _socket; }
	NetThread* getNetThread() { return _socketThread;}
	asio::ip::address GetRemoteIpAddress() const { return _remoteAddress; }
	uint16 GetRemotePort() const { return _remotePort; }
protected:
	void loopRead();
	void handleRead(const std::error_code& /*error*/, size_t /*bytes_transferred*/);
	void handleAsyncConnect(const std::error_code& error);
	virtual void onMessage(MessageBuffer & buffer) = 0;
	void shutdown(bool isauto = true);
	bool sending();
	void asyncWrite();
	void handlerWrite(const std::error_code& error, std::size_t transferedBytes);
	virtual bool makeSendBuffer(MessageBuffer& buffer) = 0;
	virtual void onAccept() {}
	virtual void onConnect(const std::error_code & error) {}
	virtual void onClose() {}

protected:
	asio::ip::tcp::socket _socket;
	asio::ip::address _remoteAddress;
	uint16 _remotePort;

	MessageBuffer _readBuffer;
	MessageBuffer _writeBuffer;
	NetThread* _socketThread;
	std::atomic<bool> _closed;
	std::atomic<bool> _waitclose;
	bool _sending;
};

typedef std::shared_ptr<Socket> SocketPtr;

