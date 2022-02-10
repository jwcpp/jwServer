#pragma once

#include "NetThread.h"
#include "SpinLock.h"
class SocketMgr
{
public:
	SocketMgr(int threadCount);
	virtual ~SocketMgr();

	void start();
	void stop();
	void wait();
	NetThread* next();
	NetThread* get(int index);
private:
	NetThread * _threads;
	int _threadCount;
	int _pos;
	SpinLock _lock;
};

