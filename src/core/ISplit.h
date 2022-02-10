#pragma once

#include <sstream>
#include <vector>

template <typename T = std::string>
class ISplit
{
	typedef typename std::vector<T>::iterator iterator;
	typedef typename std::vector<T>::size_type size_type;
public:
	ISplit(const char* pstr, char sep)
	{
		split(pstr, sep);
	}

	ISplit(const std::string& str, char sep)
	{
		split(str.c_str(), sep);
	}

	void split(const char* pstr, char sep)
	{
		std::stringstream sstream(pstr);
		std::string item;
		while (std::getline(sstream, item, sep))
		{
			T value = T();
			std::stringstream tmp(item);
			tmp >> value;

			m_splits.push_back(value);
		}
	}

	iterator begin()
	{
		return std::move(m_splits.begin());
	}
	iterator end()
	{
		return std::move(m_splits.end());
	}

	T& operator [] (int index)
	{
		auto iter = begin();

		for (int i = 0; i < index; ++i, ++iter) {}

		return (*iter);
	}

	std::vector<T>& get()
	{
		return m_splits;
	}

	std::vector<T> move()
	{
		return std::move(m_splits);
	}

	size_type size()
	{
		return m_splits.size();
	}

private:
	ISplit(const ISplit&);
	ISplit& operator=(const ISplit&);

private:
	std::vector<T> m_splits;
};