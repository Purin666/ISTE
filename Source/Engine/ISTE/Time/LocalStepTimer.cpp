#include "LocalStepTimer.h"

void ISTE::LocalStepTimer::Update(const float aTimeDelta)
{
	if (!myOnFlag)
		return;

	// tick
	myElapsedTime += aTimeDelta;

	// process
	if (myDelay < myElapsedTime)
	{
		myCallback();
		myElapsedTime = 0.f;
	}
}
