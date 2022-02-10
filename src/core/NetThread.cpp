#include "NetThread.h"
#include "Util.h"

NetThread::NetThread() :_ioContext(1), _stopped(false), _thread(nullptr), _updateTimer(_ioContext), _threadId(0)
{

}

NetThread::~NetThread()
{
	if (_thread)
	{
		delete _thread;
		_thread = nullptr;
	}
}


void NetThread::stop()
{
	_stopped = true;
	_ioContext.stop();
}

bool NetThread::start()
{
	if (_thread)
		return false;

	_thread = new std::thread(&NetThread::run, this);
	return true;
}

void NetThread::wait()
{
	_thread->join();
}

bool NetThread::isSame()
{
	return _threadId == Util::getTid();
}

void NetThread::accept(std::shared_ptr<Socket>& socket)
{
	if (isSame())
	{
		socket->start();
		_sockets.push_back(socket);
	}
	else
	{
		_ioContext.post([this, socket] {
			socket->start();
			_sockets.push_back(socket);
			});
	}
}

void NetThread::connect(std::shared_ptr<Socket>&& socket)
{
	_sockets.push_back(socket);
}

void NetThread::run()
{
	_threadId = Util::getTid();
	_updateTimer.expires_from_now(asio::chrono::milliseconds(10));
	_updateTimer.async_wait(std::bind(&NetThread::update, this));
	_ioContext.run();

	_sockets.clear();
}

void NetThread::update()
{
	if (_stopped)
		return;

	_updateTimer.expires_from_now(asio::chrono::milliseconds(10));
	_updateTimer.async_wait(std::bind(&NetThread::update, this));

	_sockets.erase(std::remove_if(_sockets.begin(), _sockets.end(), [this](std::shared_ptr<Socket> sock)
		{
			if (!sock->update())
			{
				// remove

				return true;
			}

			return false;
		}), _sockets.end());
}