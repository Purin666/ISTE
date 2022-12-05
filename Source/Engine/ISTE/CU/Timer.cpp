#include "Timer.h"

#include "ISTE/CU/MemTrack.hpp"

CU::Timer::Timer() :
	myInitTime(steady_clock::now()),
	myLastTick(steady_clock::now()),
	myCurrentTime(steady_clock::now()),
	myDeltaTime(static_cast<float>(steady_clock::now().time_since_epoch().count())),
	myTotalTime(double())
{}


void CU::Timer::Update()
{
	myCurrentTime = steady_clock::now();

	myDeltaTime = static_cast<float>(myCurrentTime.time_since_epoch().count() - myLastTick.time_since_epoch().count());
	myTotalTime = static_cast<float>(steady_clock::now().time_since_epoch().count() - myInitTime.time_since_epoch().count());

	myLastTick = steady_clock::now();
}

float CU::Timer::GetDeltaTime() const
{
	return myDeltaTime / powf(10, 9);
}

float CU::Timer::GetTotalTime() const
{
	return myTotalTime / powf(10, 9);
}
