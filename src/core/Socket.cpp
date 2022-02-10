#include "Socket.h"
#include "NetThread.h"

#define READ_BLOCK_SIZE 4096

Socket::Socket(NetThread* socketThread) :
	_socket(socketThread->getIoContext()),
	_socketThread(socketThread),
	_closed(false),
	_waitclose(false),
	_sending(false),
	_remotePort(0)
{
	_readBuffer.Resize(READ_BLOCK_SIZE);
}

Socket::~Socket()
{
	_closed = true;
	asio::error_code error_;
	_socket.close(error_);
}

void Socket::start()
{
	asio::ip::tcp::socket::endpoint_type endPoint = _socket.remote_endpoint();
	_remoteAddress = endPoint.address();
	_remotePort = endPoint.port();

	loopRead();
	onAccept();
}

void Socket::loopRead()
{
	_readBuffer.Reset();
	_socket.async_read_some(asio::buffer(_readBuffer.GetWritePointer(), _readBuffer.GetRemainingSpace()),
		std::bind(&Socket::handleRead, shared_from_this(),
			std::placeholders::_1,
			std::placeholders::_2));
}

void Socket::handleRead(const std::error_code& error, size_t bytes_transferred)
{
	if (error)
	{
		shutdown();
		return;
	}

	_readBuffer.WriteCompleted(bytes_transferred);

	onMessage(_readBuffer);

	loopRead();
}

void Socket::connect(const char* ip, uint16 port)
{
	//asio::ip::address addr = asio::ip::address::from_string(ip); // addr.is_v4()	
	connect(asio::ip::address::from_string(ip), port);
}

void Socket::connect(asio::ip::address addr, uint16 port)
{
	_remotePort = port;
	_remoteAddress = addr;

	//_remoteAddress.to_v4().to_string();
	asio::ip::tcp::endpoint endpoint_(addr, port);

	_socket.async_connect(endpoint_,
		std::bind(&Socket::handleAsyncConnect,
			shared_from_this(),
			std::placeholders::_1));
}

void Socket::close()
{
	shutdown(false);
}

void Socket::waitClose()
{
	_waitclose = true;
}

bool Socket::update()
{
	if (_closed) return false;

	asyncWrite();
	return true;
}

void Socket::handleAsyncConnect(const std::error_code& error)
{
	if (!error)
	{
		loopRead();
		_socketThread->connect(shared_from_this());
	}
	/*
	else
	{
		std::string info = error.message();
		printf("%s\n", info.c_str());
	}
	*/

	onConnect(error);
}

void Socket::shutdown(bool isauto)
{
	if (_closed.exchange(true))
		return;

	asio::error_code error_;
	_socket.shutdown(asio::socket_base::shutdown_both, error_);
	if (error_)
	{
		printf("shutdown error:[%d] %s", error_.value(), error_.message().c_str());
	}

	if (isauto) onClose();
}

bool Socket::sending()
{
	return _sending;
}

void Socket::asyncWrite()
{
	if (_sending) return;
	if (_writeBuffer.GetActiveSize() == 0 && !makeSendBuffer(_writeBuffer))
	{
		if (_waitclose)
		{
			shutdown();
		}
		return;
	}

	_sending = true;
	_socket.async_write_some(asio::buffer(_writeBuffer.GetReadPointer(), _writeBuffer.GetActiveSize()), std::bind(&Socket::handlerWrite,
		this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
}

void Socket::handlerWrite(const std::error_code& error, std::size_t transferedBytes)
{
	if (error)
	{
		shutdown();
		return;
	}

	_sending = false;
	_writeBuffer.ReadCompleted(transferedBytes);
	asyncWrite();
}