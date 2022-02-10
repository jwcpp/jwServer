#pragma once

#include "Socket.h"

class NetThread
{
public:
	NetThread();

	virtual ~NetThread();

	bool start();
	void stop();
	void wait();
	bool isSame();
	void accept(std::shared_ptr<Socket> & socket);
	void connect(std::shared_ptr<Socket> && socket);

	asio::io_context* getIoPtr() { return &_ioContext; }
	asio::io_context& getIoContext() { return _ioContext;}

protected:
	void run();

	void update();

private:
	typedef std::list<std::shared_ptr<Socket>> SocketContainer;

	asio::io_context _ioContext;
	std::atomic<bool> _stopped;
	SocketContainer _sockets;

	std::thread* _thread;
	uint32 _threadId;
	asio::steady_timer _updateTimer;

};