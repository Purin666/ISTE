#include "LocalCountDown.h"

void ISTE::LocalCountDown::Update(const float aTimeDelta)
{
	if (!myOnFlag)
		return;

	// tick
	myRemainingTime -= aTimeDelta;

	// process
	if (myRemainingTime < 0.f)
	{
		myCallback();
		myRemainingTime = myDuration;

		if (!myRepeatFlag)
			myOnFlag = false;
	}
}
