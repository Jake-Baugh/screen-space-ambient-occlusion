#include "timer.h"

Timer::Timer() {

}

Timer::~Timer() {

}

bool Timer::Initialize() {
	//check if we support high performance timers
	QueryPerformanceFrequency((LARGE_INTEGER*)&m_frequency);
	if(m_frequency == 0) {
		return false;
	}

	m_ticksPerMs = (float)(m_frequency / 1000);

	QueryPerformanceCounter((LARGE_INTEGER*)&m_startTime);
	m_begin = m_startTime;
	return true;
}

void Timer::Frame() {
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)& currentTime);

	timeDifference = (float)(currentTime - m_startTime);

	m_frameTime = timeDifference / m_ticksPerMs;

	m_startTime = currentTime;

	return;
}

float Timer::GetTime() { return m_frameTime; 
}
float Timer::GetTotalElapsedTime() { 
	INT64 currentTime;
	float timeDifference;

	QueryPerformanceCounter((LARGE_INTEGER*)& currentTime);
	timeDifference = (float)(currentTime - m_begin);
	m_totalElapsedTimeSec = timeDifference / m_ticksPerMs / 1000.f;
	return m_totalElapsedTimeSec;
}