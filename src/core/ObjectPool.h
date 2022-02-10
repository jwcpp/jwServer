#pragma once
#include <forward_list>

#define DefPoolObj()\
public:\
	bool & isInPool(){\
		return _isInPool;\
	}\
private:\
	bool _isInPool = false;

template<typename T>
class ObjectPool
{
public:
	~ObjectPool() {
		destroyAll();
	}

	T* create()
	{
		T* obj = NULL;
		if (!_list.empty()) {
			obj = _list.front();
			_list.pop_front();
			obj->isInPool() = false;
			--_count;
		}
		else
		{
			obj = new T;
		}
		obj->clear();
		return obj;
	}

	template <typename... Arg>
	T* create(Arg... arg)
	{
		T* obj = NULL;
		if (!_list.empty()) {
			obj = _list.front();
			_list.pop_front();
			obj->isInPool() = false;
			--_count;
		}
		else
		{
			obj = new T(arg...);
		}
		obj->clear();
		return obj;
	}

	void free(T * obj)
	{
		if (!obj->isInPool()) {
			obj->isInPool() = true;
			_list.push_front(obj);
			++_count;
		}
	}

	int count() {
		return _count;
	}

	void destroy(int count_){
		while (!_list.empty() && count_ --)
		{
			T* obj = _list.front();
			_list.pop_front();
			delete obj;
			--_count;
		}

	}

	void destroyAll(){
		while (!_list.empty())
		{
			T* obj = _list.front();
			_list.pop_front();
			delete obj;
		}
		_count = 0;
	}

private:
	int _count = 0;
	std::forward_list<T*> _list;
};