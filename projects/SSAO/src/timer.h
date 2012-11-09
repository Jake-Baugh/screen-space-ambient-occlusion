#ifndef _timer_h_
#define _timer_h_

#include <windows.h>

class Timer 
{
public:
	Timer();
	~Timer();

	bool Initialize();
	void Frame();
	float GetTime();
	float GetTotalElapsedTime();

private:
	INT64 m_frequency;
	float m_ticksPerMs;
	INT64 m_startTime;
	float m_frameTime;
	INT64 m_begin;
	float m_totalElapsedTimeSec;

};

#endif