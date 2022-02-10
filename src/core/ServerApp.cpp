#include "ServerApp.h"
#include "Timer.h"
#include <thread>
#include "GameTime.h"

ServerApp::ServerApp():
	m_iDiff(0)
{
	GameTime::UpdateGameTimers();
	tm const* pTm = GameTime::GetDateAndTime();
	m_iLastSecond = pTm->tm_sec;
	m_iLastMinute = pTm->tm_min;
	m_iLastHour = pTm->tm_hour;
	m_iLastDay = pTm->tm_mday;
}

int ServerApp::run(uint32 sleep_ms)
{
	uint32 currTime = 0;
	uint32 prevTime = getMSTime();
	while (!isStopped())
	{
		currTime = getMSTime();
		update(getMSTimeDiff(prevTime, currTime));
		prevTime = currTime;

		uint32 updateDiff = getMSTimeDiff(currTime, getMSTime());
		if (updateDiff < sleep_ms)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(sleep_ms - updateDiff));
		}
	}

	return 0;
}

#define TICK_MS 50
void ServerApp::update(uint32 diff)
{
	onUpdate(diff);

	m_iDiff += diff;
	if (m_iDiff >= TICK_MS)
	{
		onTick(m_iDiff);
		m_iDiff = 0;

		// update time
		GameTime::UpdateGameTimers();
		tm const*  pTm = GameTime::GetDateAndTime();
		if (pTm->tm_sec != m_iLastSecond)
		{
			m_iLastSecond = pTm->tm_sec;
			onSecond();
		}
		if (pTm->tm_min != m_iLastMinute)
		{
			m_iLastMinute = pTm->tm_min;
			onMinute();
		}
		if (pTm->tm_hour != m_iLastHour)
		{
			m_iLastHour = pTm->tm_hour;
			onHour();
		}
		if (pTm->tm_mday != m_iLastDay)
		{
			m_iLastDay = pTm->tm_mday;
			onDay();
		}
	}
}