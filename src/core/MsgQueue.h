#pragma once

#include <list>
#include <mutex>

template<typename T>
class MsgQueue
{
public:
	MsgQueue(){
		_msgRecv = &_msgQueue[0];
		_msgDoing = &_msgQueue[1];
	}

	void push(T & msg)
	{
		push(std::move(msg));
	}
	void push(T && msg)
	{
		std::lock_guard<std::mutex> lock(_lock);
		_msgRecv->push_back(msg);
	}
	void swap()
	{
		std::lock_guard<std::mutex> lock(_lock);
		std::list<T>* tmp = _msgDoing;
		_msgDoing = _msgRecv;
		_msgRecv = tmp;
	}

	bool pop(T & msg)
	{
		if (_msgDoing->empty())
		{
			return false;
		}

		msg = std::move(_msgDoing->front());
		_msgDoing->pop_front();
		return true;
	}
private:
	std::list<T>  _msgQueue[2];
	std::list<T>* _msgRecv;
	std::list<T>* _msgDoing;

	std::mutex _lock;
};

