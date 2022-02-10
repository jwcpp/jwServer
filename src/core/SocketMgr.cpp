#include "SocketMgr.h"

SocketMgr::SocketMgr(int threadCount)
{
	_threads = new NetThread[threadCount];
	_threadCount = threadCount;
	_pos = 0;
}

SocketMgr::~SocketMgr()
{
	if (_threads)
	{
		delete[] _threads;
		_threads = nullptr;
	}
}

void SocketMgr::start()
{
	for (int i = 0; i < _threadCount; ++i)
	{
		_threads[i].start();
	}
}

void SocketMgr::stop()
{
	for (int i = 0; i< _threadCount; ++i)
	{
		_threads[i].stop();
	}
}

void SocketMgr::wait()
{
	for (int i = 0; i < _threadCount; ++i)
	{
		_threads[i].wait();
	}
}

NetThread* SocketMgr::next()
{
	_lock.lock();
	int idx = _pos++;
	if (_pos == _threadCount)
	{
		_pos = 0;
	}
	_lock.unlock();

	return &_threads[idx];
}

NetThread* SocketMgr::get(int index)
{
	if (index < 0 || index >= _threadCount)return nullptr;

	return &_threads[index];
}