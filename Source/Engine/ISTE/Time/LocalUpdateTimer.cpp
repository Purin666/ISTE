#include "LocalUpdateTimer.h"

void ISTE::LocalUpdateTimer::Update(const float aTimeDelta)
{
	if (!myOnFlag)
		return;

	// tick
	myElapsedTime += aTimeDelta;
	++mySkippedFrames;

	// process
	if (myFramesToSkip < mySkippedFrames)
	{
		myCallback();
		mySkippedFrames = 0;
	}
	if (myDuration < myElapsedTime)
		myOnFlag = false;
}
