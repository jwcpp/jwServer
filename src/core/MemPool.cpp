#include "MemPool.h"

MemRecord::MemRecord(size_t partition, size_t back) :
	mPartition(partition), mBackSize(back)
{
	
}

// vector copy ==> Small space to large space
MemRecord::MemRecord(MemRecord&& obj):
	mPartition(obj.mPartition),
	mBackSize(obj.mBackSize),
	mBlocks(std::move(obj.mBlocks))
{
	first = obj.first;
}

MemRecord::~MemRecord()
{
	for (auto it = mBlocks.begin(); it != mBlocks.end(); ++it)
	{
		::free(*it);
	}
}

void MemRecord::addBlock()
{
	auto ptr = ::malloc(mBackSize);
	boost::simple_segregated_storage<size_t>::add_block(ptr, mBackSize, mPartition);
	mBlocks.push_back(ptr);
}

void* MemRecord::create()
{
	if (empty()) return NULL;
	return boost::simple_segregated_storage<size_t>::malloc();
}

void MemRecord::destroy(void* ptr)
{
	if (!ptr) return;
	boost::simple_segregated_storage<rsize_t>::free(ptr);
}

size_t MemRecord::partition()
{
	return mPartition;
}





MemPool::~MemPool() {
	clear();
}

void MemPool::init()
{
#if 1
	// 2倍
	//2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536
	for (int i = 1; i <= 16; ++i)
	{
		int size = 1 << i;
		if (size >= 8)
		{
			MemRecord& record = mBlocks.emplace_back(size, (size < 128 ? 32768 : 65536));
			record.addBlock();
			//record.addBlock();
		}
	}

#endif
}

void MemPool::clear() {
	mBlocks.clear();
	mUseinfo.clear();
}


void* MemPool::create(size_t size) {
	if (size == 0) return NULL;

	int idx = binarySearch(size);
	if (idx < mBlocks.size())
	{
		// lock
		std::lock_guard<std::mutex> lock(m_lock);
		void* ptr = mBlocks[idx].create();
		if (ptr) {
			mUseinfo.emplace(ptr, idx);
			return ptr;
		}
	}

	return ::malloc(size);
}

void MemPool::destroy(void* ptr, size_t size)
{
	if (!ptr || !size) return;

	{
		// lock
		std::lock_guard<std::mutex> lock(m_lock);
		auto it = mUseinfo.find(ptr);
		if (it != mUseinfo.end())
		{
			mBlocks[it->second].destroy(ptr);
			mUseinfo.erase(it);
			return;
		}
	}
	::free(ptr);
}

//查找目标值的右边界
int MemPool::binarySearch(size_t target) {
	int left = 0, right = mBlocks.size();
	while (left < right)
	{
		int mid = (left + right) / 2;
		size_t value = mBlocks[mid].partition();
		if (value == target) {
			right = mid;
		}
		else if (value > target) {
			right = mid;
		}
		else if (value < target) {
			left = mid + 1;
		}
	}
	return left;
}