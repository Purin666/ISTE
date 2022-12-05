#pragma once

#include "TimerDefines.h"

#include <bitset>
#include <string>

#define MAX_TIMERS 256

namespace ISTE
{
	struct TimerPool
	{
		CountDown countDowns[MAX_TIMERS];
		StopWatch stopWatches[MAX_TIMERS];
		StepTimer stepTimers[MAX_TIMERS];
		UpdateTimer updateTimers[MAX_TIMERS];

		std::bitset<MAX_TIMERS>	repeatingIsOn; // only for CountDown
		std::bitset<MAX_TIMERS>	countDownIsOn;
		std::bitset<MAX_TIMERS>	stopWatchIsOn;
		std::bitset<MAX_TIMERS>	stepTimerIsOn;
		std::bitset<MAX_TIMERS>	updateTimerIsOn;

		std::bitset<MAX_TIMERS>	countDownPaused;
		std::bitset<MAX_TIMERS>	stopWatchPaused;
		std::bitset<MAX_TIMERS>	stepTimerPaused;
		std::bitset<MAX_TIMERS>	updateTimerPaused;
	};

	void AddTimer(TimerPool& aPool, const CountDown& aTimer);
	void AddTimer(TimerPool& aPool, const StopWatch& aTimer);
	void AddTimer(TimerPool& aPool, const StepTimer& aTimer);
	void AddTimer(TimerPool& aPool, const UpdateTimer& aTimer);

	void RemoveTimer(TimerPool& aPool, const size_t anId, TimerType aType);
	void RemoveTimer(TimerPool& aPool, const std::string& aName);
	void RemoveAllTimers(TimerPool& aPool);

	const float GetTime(const TimerPool& aPool, const std::string& aName);

	void SetOn(TimerPool& aPool, const std::string& aName, const bool aFlag);

	void PauseAllActiveTimers(TimerPool& aPool, std::bitset<MAX_TIMERS>&, std::bitset<MAX_TIMERS>&, std::bitset<MAX_TIMERS>&, std::bitset<MAX_TIMERS>&);
	void ResumeAllPausedTimers(TimerPool& aPool, std::bitset<MAX_TIMERS>&, std::bitset<MAX_TIMERS>&, std::bitset<MAX_TIMERS>&, std::bitset<MAX_TIMERS>&);

	void TickTimers(TimerPool& aPool, const float aTimeDelta);
	void ProcessTimers(TimerPool& aPool);
	
	void InvokeTimer(TimerPool& aPool, const size_t anId, ISTE::TimerType aType);
	void InvokeTimer(TimerPool& aPool, const std::string& aName);
	void InvokeAllTimers(TimerPool& aPool);

}