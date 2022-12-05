// covered in TimeHandler

//#include "StepTimer.h"
//
//void CU::StepTimer::Init(const size_t someUpdatesPerSecond, std::function<void()> aCallBack)
//{
//	myCallBack = aCallBack;
//	myUpdateDelay = 1 / (float)someUpdatesPerSecond;
//}
//
//const bool CU::StepTimer::Update(const float aTimeDelta)
//{
//	myElapsedTime += aTimeDelta;
//	if (myUpdateDelay < myElapsedTime)
//	{
//		if (myCallBack) myCallBack();
//		myElapsedTime = 0.f;
//		return true;
//	}
//	return false;
//}
