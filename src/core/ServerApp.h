#pragma once
#include "BaseType.h"

class ServerApp
{
public:
	ServerApp();
	virtual ~ServerApp() {}

	virtual int run(uint32 sleep_ms = 10);
	
protected:
	virtual bool isStopped() { return false; }
	
	virtual void onUpdate(uint32 diff) {}
	virtual void onTick(uint32 diff) {}
	virtual void onSecond() {}
	virtual void onMinute() {}
	virtual void onHour() {}
	virtual void onDay() {}

private:
	void update(uint32 diff);

	uint32 m_iDiff;
	int  m_iLastSecond;
	int  m_iLastMinute;
	int  m_iLastHour;
	int  m_iLastDay;
};