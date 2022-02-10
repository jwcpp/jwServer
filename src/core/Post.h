#pragma once
#include <list>
#include "SpinLock.h"

class PostBase
{
public:
	virtual ~PostBase() {}
	virtual void call() = 0;
};

template<typename FUNC>
class PostNone : public PostBase
{
public:
	PostNone(FUNC && func):
		m_func(std::forward<FUNC>(func)){
	}

	virtual void call() {
		m_func();
	}
private:
	FUNC m_func;
};

#include <tuple>

template<typename FUNC, typename... ARGS>
class PostArge : public PostBase
{
public:
	PostArge(FUNC&& func, ARGS &&...args):
	m_func(std::forward<FUNC>(func)),
	m_tuple(std::forward<ARGS>(args)...){
	
	}
	virtual void call() {
		std::apply(std::move(m_func), std::move(m_tuple));
	}

private:
	FUNC m_func;
	std::tuple<ARGS...> m_tuple;
};

class Post
{
public:
	~Post();

	template<typename FUNC>
	void post(FUNC && func) {
		PostBase* node = new PostNone<FUNC>(std::forward<FUNC>(func));
		add(node);
	}

	template<typename FUNC, typename... ARGS>
	void post(FUNC&& func, ARGS &&...args) {
		PostBase* node = new PostArge<FUNC, ARGS...>(std::forward<FUNC>(func), std::forward<ARGS>(args)...);
		add(node);
	}

	void add(PostBase* node);
	void update();

private:
	SpinLock mLock;
	std::list<PostBase*> mPostlist;
};