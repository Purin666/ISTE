#include "TimeHandler.h"
#include "TimerPool.h"
#include "ISTE/CU/Timer.h"
#include "ISTE/CU/MemTrack.hpp"

const float ISTE::TimeHandler::GetDeltaTime() const
{
	return myCUTimer->GetDeltaTime() * myTimeScale;
}
const float ISTE::TimeHandler::GetTotalTime() const
{
	return myCUTimer->GetTotalTime();
}

const float ISTE::TimeHandler::GetTime(const std::string& aName)
{
	return ISTE::GetTime(*myPool, aName);
}

void ISTE::TimeHandler::ResetDeltaTime()
{
	return myCUTimer->ResetDeltaTime();
}

void ISTE::TimeHandler::AddTimer(const CountDown& aTimer)
{
	ISTE::AddTimer(*myPool, aTimer);
}
void ISTE::TimeHandler::AddTimer(const StopWatch& aTimer)
{
	ISTE::AddTimer(*myPool, aTimer);
}
void ISTE::TimeHandler::AddTimer(const StepTimer& aTimer)
{
	ISTE::AddTimer(*myPool, aTimer);
}

void ISTE::TimeHandler::AddTimer(const UpdateTimer& aTimer)
{
	ISTE::AddTimer(*myPool, aTimer);
}

void ISTE::TimeHandler::RemoveTimer(const std::string& aName)
{
	ISTE::RemoveTimer(*myPool, aName);
}
void ISTE::TimeHandler::RemoveAllTimers()
{
	ISTE::RemoveAllTimers(*myPool);
}
void ISTE::TimeHandler::SetOn(const std::string& aName, bool aFlag)
{
	ISTE::SetOn(*myPool, aName, aFlag);
}

void ISTE::TimeHandler::PauseAllActiveTimers()
{
	ISTE::PauseAllActiveTimers(
		*myPool,
		myPool->countDownPaused,
		myPool->stopWatchPaused,
		myPool->stepTimerPaused,
		myPool->updateTimerPaused
	);
}

void ISTE::TimeHandler::ResumeAllPausedTimers()
{
	ISTE::ResumeAllPausedTimers(
		*myPool,
		myPool->countDownPaused,
		myPool->stopWatchPaused,
		myPool->stepTimerPaused,
		myPool->updateTimerPaused
	);
}

void ISTE::TimeHandler::TickTimers()
{
	myCUTimer->Update(); // TODO:: Maybe integrate CU::Timer with the handler // Mathias
	ISTE::TickTimers(*myPool, myCUTimer->GetDeltaTime());
}
void ISTE::TimeHandler::ProcessTimers()
{
	ISTE::ProcessTimers(*myPool);
}

void ISTE::TimeHandler::InvokeTimer(const std::string& aName)
{
	ISTE::InvokeTimer(*myPool, aName);
}
void ISTE::TimeHandler::InvokeAllTimers()
{
	ISTE::InvokeAllTimers(*myPool);
}
