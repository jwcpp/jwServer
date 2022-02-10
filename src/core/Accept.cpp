#include "Accept.h"
#include "NetThread.h"

Accept::Accept(std::function<SocketPtr()> createfunc, std::function<bool(SocketPtr)> acceptfunc)
{
	_acceptor = nullptr;
	_createfunc = createfunc;
	_acceptfunc = acceptfunc;
	_closed = false;
}

Accept::~Accept()
{
	if (_acceptor)
	{
		delete _acceptor;
		_acceptor = nullptr;
	}
}

void Accept::start(const char* ip, unsigned short port, NetThread* net_thread)
{
	asio::ip::address addr = asio::ip::address::from_string(ip); // addr.is_v4()
	_acceptor = new asio::ip::tcp::acceptor(net_thread->getIoContext(), asio::ip::tcp::endpoint(addr, port));

	startAccept();
}

void Accept::close()
{
	if (_closed.exchange(true))
		return;

	_acceptor->close();
}

void Accept::startAccept()
{

	//asio::io_context* io = IOManager::Instance()->getAsIo();
	//Connection::Pointer new_connection = Connection::create(*io);

	SocketPtr conn = _createfunc();
	_acceptor->async_accept(conn->socket(),
		std::bind(&Accept::handleAccept, this, conn,
			std::placeholders::_1));
}

void Accept::handleAccept(SocketPtr connect, const std::error_code& error)
{
	if (!error)
	{
		if (!_acceptfunc || _acceptfunc(connect))
		{
			connect->getNetThread()->accept(connect);
			//connect->start();
		}
	}

	if(!_closed) startAccept();
}