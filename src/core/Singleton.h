#pragma once
#include <cassert>

namespace Util
{
	class NonCopyable
	{
	protected:
		NonCopyable()
		{
		}

	private:
		NonCopyable(const NonCopyable&);
		NonCopyable& operator=(const NonCopyable&);
	};

	template<class T>
	class Singleton : NonCopyable
	{
	public:
		Singleton()
		{
			assert(!m_instance);
			m_instance = static_cast<T*>(this);
		}

		~Singleton()
		{
			assert(m_instance);
			m_instance = 0;
		}

		static T* getInstance()
		{
			return m_instance;
		}

	private:
		static T* m_instance;
	};
	template<class T>
	T* Singleton<T>::m_instance = NULL;
}