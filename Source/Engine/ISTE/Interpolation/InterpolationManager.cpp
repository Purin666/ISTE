//#include "InterpolationManager.h"
//#include "ISTE/Context.h"
//#include "ISTE/Time/TimeHandler.h"
//
//void ISTE::InterpolationManager::Update()
//{
//	TickLinear();
//	TickSin();
//
//	CalcLinear();
//	CalcSin();
//}
//
//float ISTE::InterpolationManager::GetInterpValue(InterpolationID anID)
//{
//	//ftable please
//	switch (anID.myType)
//	{
//	case InterpolationType::eConstant:
//		return myConstantData[anID.myDataIndex].myTValue;
//
//	case InterpolationType::eLinear:
//		return myLinearData[anID.myDataIndex].myTValue;
//
//	case InterpolationType::eSin:
//		return mySinData[anID.myDataIndex].myTValue;
//		
//	default:	//please dont input eCount, thx
//		return 0;
//	}
//}
//
//ISTE::InterpolationID ISTE::InterpolationManager::Create(InterpolationData::ConstantData aData, InterpolationFlag aFlag)
//{
//	InterpolationID id;
//	id.myDataIndex = myConstantData.size();
//	id.myType = InterpolationType::eConstant;
//
//	myConstantData.emplace_back(aData);
//	return id;
//}
//
//ISTE::InterpolationID ISTE::InterpolationManager::Create(InterpolationData::LinearData aData, InterpolationFlag aFlag)
//{
//	InterpolationID id;
//	id.myDataIndex = myConstantData.size();
//	id.myType = InterpolationType::eConstant;
//
//	myLinearData.emplace_back(aData);
//	return id;
//}
//
//ISTE::InterpolationID ISTE::InterpolationManager::Create(InterpolationData::SinData, InterpolationFlag aFlag)
//{
//	return InterpolationID();
//}
//
//void ISTE::InterpolationManager::Remove(InterpolationID anID)
//{
//	//im making andre cry
//	switch (anID.myType)
//	{
//	case InterpolationType::eConstant:
//		myNextFreeSpotConstant.push(anID.myDataIndex);
//		return; 
//
//	case InterpolationType::eLinear:
//		myNextFreeSpotLinear.push(anID.myDataIndex); 
//
//	case InterpolationType::eSin:
//		myNextFreeSpotSin.push(anID.myDataIndex); 
//
//	default:	//please dont input eCount, thx
//		return;
//	}
//}
// 
//
//ISTE::InterpolationData::ConstantData& ISTE::InterpolationManager::GetConstantInterpData(InterpolationID anID)
//{ 
//	return myConstantData[anID.myDataIndex];
//}
//
//ISTE::InterpolationData::LinearData& ISTE::InterpolationManager::GetLinearInterpData(InterpolationID anID)
//{
//	return myLinearData[anID.myDataIndex];
//}
//
//ISTE::InterpolationData::SinData& ISTE::InterpolationManager::GetSinData(InterpolationID anID)
//{
//	return mySinData[anID.myDataIndex];
//}
//
//void ISTE::InterpolationManager::TickLinear()
//{
//	float dt = Context::Get()->myTimeHandler->GetDeltaTime();
//	size_t size = myLinearData.size();
//	size_t i = 0;
//	for (; i < size; i++)
//	{
//		myLinearData[i].myTimer = myLinearData[i].myTimer >= 1.f ? 0.f : myLinearData[i].myTimer + dt;
//	}
//}
//
//void ISTE::InterpolationManager::TickSin()
//{
//	float dt = Context::Get()->myTimeHandler->GetDeltaTime();
//	size_t size = mySinData.size();
//	size_t i = 0;
//	for (i = 0; i < size; i++)
//	{
//		if (mySinData[i].myTimer >= 1.f && 
//			!(mySinData[i].myFlag & INTERP_DESTROY_ON_COMPLETION))
//		{
//			mySinData[i].myTimer += mySinData[i].myTimer + dt;
//		}
//		else if (!mySinData[i].myFlag & INTERP_DESTROY_ON_COMPLETION) {
//			mySinData[i].myTimer = 0;
//		}
//		else {
//
//		}
//	}
//}
//
//void ISTE::InterpolationManager::CalcLinear()
//{ 
//	size_t size = myLinearData.size(); 
//	size_t i = 0;
//	for (; i < size; i++)
//	{ 
//		myLinearData[i].myTValue = myLinearData[i].myTimer * myLinearData[i].mySpeed;
//	}
//}
//
//void ISTE::InterpolationManager::CalcSin()
//{ 
//	size_t size = mySinData.size();
//	size_t i = 0;
// 
//	for (i = 0; i < size; i++)
//	{
//		mySinData[i].myTValue = 0.5f + (sin(mySinData[i].myTimer * mySinData[i].mySpeed) * 0.5f);
//	}
//}