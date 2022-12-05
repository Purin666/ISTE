#include "TimerPool.h"
#include <cassert>

#ifdef _DEBUG
#include "ISTE/Logger/Logger.h"
#include "../CU/MemoryTracker.h"
#include "ISTE/CU/MemTrack.hpp"
#endif // _DEBUG

// TODO:: UpdateTimer was quick implementation. Others are OK but the whole system could definitely be better optimized. //Mathias


void ISTE::AddTimer(TimerPool& aPool, const CountDown& aTimer)
{
	for (size_t i = 0; i < MAX_TIMERS; ++i)
	{
		if (aPool.countDowns[i].callback) continue;

		aPool.countDowns[i].callback = aTimer.callback;
		aPool.countDowns[i].name = aTimer.name;
		aPool.countDowns[i].repeat = aTimer.repeat;
		aPool.countDowns[i].duration = aTimer.duration;
		aPool.countDowns[i].remainingTime = aTimer.duration;
		aPool.repeatingIsOn[i] = true;
		aPool.countDownIsOn[i] = true;
		return;
	}
	assert(false && "maximum number of active timers exceeded; please increase MAX_TIMERS");
}
void ISTE::AddTimer(TimerPool& aPool, const StopWatch& aTimer)
{
	for (size_t i = 0; i < MAX_TIMERS; ++i)
	{
		if (aPool.stopWatches[i].callback) continue;

		aPool.stopWatches[i].callback = aTimer.callback;
		aPool.stopWatches[i].name = aTimer.name;
		aPool.stopWatches[i].invokeTime = aTimer.invokeTime;
		aPool.stopWatchIsOn[i] = true;
		return;
	}
	assert(false && "maximum number of active timers exceeded; please increase MAX_TIMERS");
}
void ISTE::AddTimer(TimerPool& aPool, const StepTimer& aTimer)
{
	assert(aTimer.callback && "tried to add steptimer with nullptr as callback; please don't send nullptr");
	for (size_t i = 0; i < MAX_TIMERS; ++i)
	{
		if (aPool.stepTimers[i].callback) continue;

		aPool.stepTimers[i].callback = aTimer.callback;
		aPool.stepTimers[i].name = aTimer.name;
		aPool.stepTimers[i].delay = aTimer.delay;
		aPool.stepTimerIsOn[i] = true;
		return;
	}
	assert(false && "maximum number of active timers exceeded; please increase MAX_TIMERS");
}
void ISTE::AddTimer(TimerPool& aPool, const UpdateTimer& aTimer)
{
	assert(aTimer.callback && "tried to add UpdateTimer with nullptr as callback; please don't send nullptr");
	for (size_t i = 0; i < MAX_TIMERS; ++i)
	{
		if (aPool.updateTimers[i].callback) continue;

		aPool.updateTimers[i].callback = aTimer.callback;
		aPool.updateTimers[i].name = aTimer.name;
		aPool.updateTimers[i].framesToSkip = aTimer.framesToSkip;
		aPool.updateTimers[i].duration = aTimer.duration;
		aPool.updateTimers[i].skipped = aTimer.framesToSkip;
		aPool.updateTimerIsOn[i] = true;
		return;
	}
	assert(false && "maximum number of active timers exceeded; please increase MAX_TIMERS");
}
void ISTE::RemoveTimer(TimerPool& aPool, const size_t anId, ISTE::TimerType aType)
{
	assert(anId < MAX_TIMERS && "timer ID out of range");
	switch (aType)
	{
	case ISTE::TimerType::eCountDown:
		aPool.countDowns[anId].callback = nullptr;
		aPool.countDowns[anId].name = "Untitled CountDown";
		aPool.countDowns[anId].repeat = false;
		aPool.countDowns[anId].duration = FLT_MAX;
		aPool.repeatingIsOn[anId] = false;
		aPool.countDownIsOn[anId] = false;
		break;
	case ISTE::TimerType::eStopWatch:
		aPool.stopWatches[anId].callback = nullptr;
		aPool.stopWatches[anId].name = "Untitled StopWatch";
		aPool.stopWatches[anId].invokeTime = FLT_MAX;
		aPool.stopWatches[anId].elapsedTime = 0.f;
		aPool.stopWatchIsOn[anId] = false;
		break;
	case ISTE::TimerType::eStepTimer:
		aPool.stepTimers[anId].callback = nullptr;
		aPool.stepTimers[anId].name = "Untitled CountDown";
		aPool.stepTimers[anId].delay = FLT_MAX;
		aPool.stepTimers[anId].elapsedTime = 0.f;
		aPool.stepTimerIsOn[anId] = false;
		break;
	case ISTE::TimerType::eUpdateTimer:
		aPool.updateTimers[anId].callback = nullptr;
		aPool.updateTimers[anId].name = "Untitled CountDown";
		aPool.updateTimers[anId].framesToSkip = 0;
		aPool.updateTimers[anId].elapsedTime = 0.f;
		aPool.updateTimerIsOn[anId] = false;
		break;
	}
}
void ISTE::RemoveTimer(TimerPool& aPool, const std::string& aName)
{
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.countDowns[i].name == aName)
		{
			RemoveTimer(aPool, i, TimerType::eCountDown);
			return;
		}
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.stopWatches[i].name == aName)
		{
			RemoveTimer(aPool, i, TimerType::eStopWatch);
			return;
		}
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.stepTimers[i].name == aName)
		{
			RemoveTimer(aPool, i, TimerType::eStepTimer);
			return;
		}
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.updateTimers[i].name == aName)
		{
			RemoveTimer(aPool, i, TimerType::eUpdateTimer);
			return;
		}
}
const float ISTE::GetTime(const TimerPool& aPool, const std::string& aName)
{
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.countDowns[i].name == aName)
			return aPool.countDowns[i].remainingTime;

	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.stopWatches[i].name == aName)
			return aPool.stopWatches[i].elapsedTime;

	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.stepTimers[i].name == aName)
			return aPool.stepTimers[i].elapsedTime;

	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.updateTimers[i].name == aName)
			return aPool.updateTimers[i].elapsedTime;

	return 0.f;
}

void ISTE::RemoveAllTimers(TimerPool& aPool)
{

	std::fill(std::begin(aPool.countDowns), std::end(aPool.countDowns), CountDown());
	std::fill(std::begin(aPool.stopWatches), std::end(aPool.stopWatches), StopWatch());
	std::fill(std::begin(aPool.stepTimers), std::end(aPool.stepTimers), StepTimer());
	std::fill(std::begin(aPool.updateTimers), std::end(aPool.updateTimers), UpdateTimer());
	
	aPool.repeatingIsOn.reset();
	aPool.countDownIsOn.reset();
	aPool.stopWatchIsOn.reset();
	aPool.stepTimerIsOn.reset();
	aPool.updateTimerIsOn.reset();

}

void ISTE::SetOn(TimerPool& aPool, const std::string& aName, const bool aFlag)
{
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.countDowns[i].name == aName)
		{
			aPool.countDownIsOn[i] = true;
			return;
		}
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.stopWatches[i].name == aName)
		{
			aPool.stopWatchIsOn[i] = true;
			return;
		}
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.stepTimers[i].name == aName)
		{
			aPool.stepTimerIsOn[i] = true;
			return;
		}
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.updateTimers[i].name == aName)
		{
			aPool.updateTimerIsOn[i] = true;
			return;
		}
}
void ISTE::PauseAllActiveTimers(TimerPool& aPool, std::bitset<MAX_TIMERS>& countDowns, std::bitset<MAX_TIMERS>& stopWatches, std::bitset<MAX_TIMERS>& stepTimers, std::bitset<MAX_TIMERS>& updateTimers)
{
	countDowns   = aPool.countDownIsOn;
	stopWatches  = aPool.stopWatchIsOn;
	stepTimers   = aPool.stepTimerIsOn;
	updateTimers = aPool.updateTimerIsOn;

	aPool.countDownIsOn.reset();
	aPool.stopWatchIsOn.reset();
	aPool.stepTimerIsOn.reset();
	aPool.updateTimerIsOn.reset();
}
void ISTE::ResumeAllPausedTimers(TimerPool& aPool, std::bitset<MAX_TIMERS>& countDowns, std::bitset<MAX_TIMERS>& stopWatches, std::bitset<MAX_TIMERS>& stepTimers, std::bitset<MAX_TIMERS>& updateTimers)
{
	aPool.countDownIsOn	  = countDowns;
	aPool.stopWatchIsOn	  = stopWatches;
	aPool.stepTimerIsOn	  = stepTimers;
	aPool.updateTimerIsOn = updateTimers;

	aPool.countDownPaused.reset();
	aPool.stopWatchPaused.reset();
	aPool.stepTimerPaused.reset();
	aPool.updateTimerPaused.reset();
}
void ISTE::TickTimers(TimerPool& aPool, const float aTimeDelta)
{
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.countDownIsOn[i])
			aPool.countDowns[i].remainingTime -= aTimeDelta;
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.stopWatchIsOn[i])
			aPool.stopWatches[i].elapsedTime += aTimeDelta;
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.stepTimerIsOn[i])
			aPool.stepTimers[i].elapsedTime += aTimeDelta;
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.updateTimerIsOn[i])
		{
			aPool.updateTimers[i].skipped++;
			aPool.updateTimers[i].elapsedTime += aTimeDelta;
		}
}
void ISTE::ProcessTimers(TimerPool& aPool)
{
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.countDowns[i].remainingTime < 0.f)
		{
			aPool.countDowns[i].callback();
			aPool.countDowns[i].remainingTime = aPool.countDowns[i].duration;

			if (!aPool.countDowns[i].repeat)
				RemoveTimer(aPool, i, TimerType::eCountDown);
		}
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.stopWatches[i].invokeTime < aPool.stopWatches[i].elapsedTime)
		{
			aPool.stopWatches[i].callback();
			RemoveTimer(aPool, i, TimerType::eStopWatch);
		}
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.stepTimers[i].delay < aPool.stepTimers[i].elapsedTime)
		{
			aPool.stepTimers[i].callback();
			aPool.stepTimers[i].elapsedTime = 0.f;
		}
	for (size_t i = 0; i < MAX_TIMERS; ++i)
	{
		if (!aPool.updateTimers[i].callback)
			continue;
		if (aPool.updateTimers[i].duration < aPool.updateTimers[i].elapsedTime)
		{
			RemoveTimer(aPool, i, TimerType::eUpdateTimer);
			continue;
		}
		if (aPool.updateTimers[i].framesToSkip < aPool.updateTimers[i].skipped)
		{
			aPool.updateTimers[i].callback();
			aPool.updateTimers[i].skipped = 0;
		}
	}
}

void ISTE::InvokeTimer(TimerPool& aPool, const size_t anId, ISTE::TimerType aType)
{
	switch (aType)
	{
	case ISTE::TimerType::eCountDown:
		aPool.countDowns[anId].callback();
		break;
	case ISTE::TimerType::eStopWatch:
		aPool.stopWatches[anId].callback();
		break;
	case ISTE::TimerType::eStepTimer:
		aPool.stepTimers[anId].callback();
		break;
	case ISTE::TimerType::eUpdateTimer:
		aPool.updateTimers[anId].callback();
		break;
	}
}
void ISTE::InvokeTimer(TimerPool& aPool, const std::string& aName)
{
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.countDowns[i].name == aName)
		{
			aPool.countDowns[i].callback();
			return;
		}
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.stopWatches[i].name == aName)
		{
			aPool.stopWatches[i].callback();
			return;
		}
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.stepTimers[i].name == aName)
		{
			aPool.stepTimers[i].callback();
			return;
		}
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.updateTimers[i].name == aName)
		{
			aPool.updateTimers[i].callback();
			return;
		}
}
void ISTE::InvokeAllTimers(TimerPool& aPool)
{
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.countDowns[i].callback)
			aPool.countDowns[i].callback();
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.stopWatches[i].callback)
			aPool.stopWatches[i].callback();
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.stepTimers[i].callback)
			aPool.stepTimers[i].callback();
	for (size_t i = 0; i < MAX_TIMERS; ++i)
		if (aPool.updateTimers[i].callback)
			aPool.updateTimers[i].callback();
}
