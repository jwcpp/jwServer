#pragma once

#include "simple_segregated_storage.hpp"
#include <forward_list>
#include <cassert>

class MemNode
{
	friend class MemPool;
public:
	
	virtual ~MemNode() {}

private:

	void* operator new(size_t size, void* ptr)
	{
		return ptr;
	}

	// External delete
	void operator delete(void* ptr){}
	void operator delete(void* ptr, void* p) {}

private:
	int allot_n = 0;
};

class MemPool : protected boost::simple_segregated_storage<int>
{
public:
	MemPool(int block, int part)
	{
		m_iBlockSize = block;
		m_iPartSize = part;
	}

	~MemPool()
	{
		destroyBlocks();
	}

	void destroyBlocks()
	{
		for (auto it = m_listMem.begin(); it != m_listMem.end(); ++it)
		{
			delete[](*it);
		}

		m_listMem.clear();
	}

	void createBlock()
	{
		char* mem = new char[m_iBlockSize];
		add_ordered_block(mem, m_iBlockSize, m_iPartSize);
		m_listMem.push_front(mem);
	}

	template<typename T>
	T* createObj()
	{
		if (!empty())
		{
			int n = chunkCount(sizeof(T));
			T* obj = new (create(n)) T;
			if (obj)
			{
				MemNode* node = dynamic_cast<MemNode*>(obj);
				// Must inherit to first position
				assert(node && node == (void*)obj);
				node->allot_n = n;
				return obj;
			}
		}

		return new(::malloc(sizeof(T))) T;
	}

	void destroyObj(MemNode* node)
	{
		
		if (!node) return;

		int allot_n = node->allot_n;
		if (allot_n)
		{
			delete node;
			destroy(node, allot_n);
		}
		else
		{
			delete node;
			::free(node);
		}
	}

	// Calc the number of chunk required 
	int chunkCount(const int objsize)
	{
		int n = objsize / m_iPartSize;
		if (objsize % m_iPartSize) ++n;
		return n;
	}

	void* create(const int n)
	{
		return malloc_n(n, m_iPartSize);
	}

	void destroy(void * allot_ptr, int allot_n)
	{
		ordered_free_n(allot_ptr, allot_n, m_iPartSize);
	}
private:
	int m_iBlockSize = 0;
	int m_iPartSize = 0;
	std::forward_list<char*> m_listMem;

};