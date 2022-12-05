#include "LocalStopWatch.h"

void ISTE::LocalStopWatch::Update(const float aTimeDelta)
{
	if (!myOnFlag)
		return;

	// tick
	myElapsedTime += aTimeDelta;

	// process
	if (myInvokeTime < myElapsedTime)
	{
		myCallback();
		myOnFlag = false;
	}
}
