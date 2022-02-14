#pragma once

#include "simple_segregated_storage.hpp"
#include "Singleton.h"
#include <vector>
#include <map>
#include <mutex>

class MemRecord : protected boost::simple_segregated_storage<size_t>, public Util::NonCopyable
{
public:
	MemRecord(size_t partition, size_t back);
	MemRecord(MemRecord&&);
	~MemRecord();

	void addBlock();

	void* create();
	void destroy(void* ptr);
	size_t partition();

private:
	size_t mBackSize;
	size_t mPartition;
	std::vector<void*> mBlocks;
};

class MemPool : public Util::Singleton<MemPool>
{
public:
	~MemPool();
	void init();
	void clear();

	void* create(size_t size);
	void destroy(void* ptr, size_t size);

private:
	int binarySearch(size_t target);

private:
	std::vector<MemRecord> mBlocks;
	std::map<void*, int> mUseinfo;
	std::mutex m_lock;
};