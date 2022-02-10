#pragma once

#define DefLinkedObj(OBJ)\
public:\
	OBJ *& next(){\
		return _next_;\
	}\
private:\
	OBJ* _next_ = NULL;


template<typename T>
class LinkedList
{
	typedef void(*Callback)(T*);
public:
	LinkedList()
	{
		_head = nullptr;
		_tail = nullptr;
	}

	virtual ~LinkedList()
	{
		//destroy();
	}


	void push(T* node)
	{
		if (_tail)
		{
			_tail = (_tail->next()) = node;
		}
		else
		{
			_head = _tail = node;
		}

		if (node)node->next() = nullptr;
	}

	T* pop()
	{
		T* ret = _head;
		if (ret)
		{
			_tail = (_head = _head->next()) ? _tail : nullptr;
		}

		return ret;
	}

	T* top(){
		return _head;
	}

	bool empty() {
		return !_head;
	}

	void destroy(Callback func = [](T* node) {delete node; })
	{
		T* ret = nullptr;
		while (ret = pop())
		{
			func(ret);
		}
	}

private:
	T* _head = nullptr;
	T* _tail = nullptr;
};

