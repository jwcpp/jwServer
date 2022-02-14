#pragma once

#include "MemPool.h"

class Buffer : public Util::NonCopyable
{
public:
	Buffer(int size = 256): mSize(size), mPos(0), mData(NULL){
		if (size){
			mData = (char *)Util::Singleton<MemPool>::getInstance()->create(size);
		}
	}

	~Buffer(){
		if (mData) {
			Util::Singleton<MemPool>::getInstance()->destroy(mData, mSize);
		}
	}

	void write(const char * data, int size) {
		std::memcpy(currPtr(), data, size);
	}
	template<typename T>
	Buffer& operator<<(const T & value)
	{
		write((const char *)&value, sizeof(value));
		return *this;
	}

	template<typename T>
	Buffer& operator>>(T& value)
	{
		std::memcpy(&value, currPtr(), sizeof(value));
		return *this;
	}

	int pos() { return mPos; }
	void setPos(int pos) { mPos = pos; }
	void addPos(int pos) { mPos += pos; }
	char* currPtr() { return mData + mPos; }
	char* contents() { return mData; }
	int size() { return mSize; }

private:
	char* mData;
	int mSize;
	int mPos;
};