#pragma once

#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"
#include "ISTE/Context.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Graphics/Resources/AnimationManager.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimationDrawerSystem.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Time/LocalStopWatch.h"
#include "ISTE/Time//LocalCountDown.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"

#include "ISTE/CU/MinHeap.hpp"

#include <unordered_map>
#include <vector>


typedef unsigned char AnimationFlag;

#define AH_LOOPING		0b00000001
#define AH_ADDTOQUE		0b00000010



namespace ISTE
{


	enum class OperationType
	{
		ePartial,
		eLerp,
		eFetch,
		eCount
		//mwba
	};

	template<typename MapType>
	class AnimationHelper
	{
	public: //structs
		//this will later define the blend graph, but for now its just one type of operation + waste of data
		struct AnimationBehaviour
		{
		public:
			AnimationFlag		myAnimationFlag;
			int					myOverrideValue		= 0;
			int					myAnimationValue	= 0;
			float				mySpeed				= 10.f;
			AnimationID			myAnimation; 
		};
		struct AnimationBlendBehaviour
		{
			AnimationBlendID	myIn, myOut;
			MapType				myMappedAnimation;
			int					myInterpSpeed	= 1;
			int					myJointID		= 0;
			float				myPartialInfl	= 1.f;
			bool				myEnabled		= false;
			bool				myHasUpdated	= false;
		};

		inline AnimationHelper() {}

		inline void SetEntityID(EntityID anEntity) 
		{ 
			myEntity	= anEntity; 
			myAnimator	= Context::Get()->mySceneHandler->GetActiveScene().GetComponent<AnimatorComponent>(myEntity);
		}

		inline void MapAnimation(MapType aMapValue, AnimationID aAnimationId, AnimationFlag aFlag, int aOverrideValue, int aAnimationValue, float aSpeed = 1.f)
		{ 
			AnimationBehaviour ab;
			ab.mySpeed			= aSpeed;
			ab.myAnimation		= aAnimationId;
			ab.myAnimationFlag	= aFlag;
			ab.myOverrideValue	= aOverrideValue;
			ab.myAnimationValue = aAnimationValue;

			myMappedAnimations[aMapValue] = ab;
		}
		
		//void BlendSetFetchOp(MapType);
		//void ForceBlendSetFetchOp(MapType);
		//void RecalculateBlends();

		//currently dosn't que stuff
		void ForcePlay(MapType aMapValue);
		void Play(MapType aMapValue);
		void ForcePlayInterpelated(MapType aFrom, MapType aToMapValue, float anInterpSpeed);
		void PlayInterpelated(MapType aMapValue, float anInterpSpeed, MapType aFromMapValue = MapType(-1));
		void ForcePlayPartially(MapType aMapValue, MapType aMapToBlendWith, int aBoneIndex, float aPartialStr, bool aFlipped = false, bool aShouldClear = false );
		void PlayPartially(MapType aMapValue, MapType aMapToBlendWith, int aBoneIndex, float aPartialStr, bool aFlipped = false, bool aShouldClear = false );



		
		//these functions can be used for special cases giving a unique flag for this run instance
		void ForcePlay(MapType aMapValue, AnimationFlag aFlag)
		{

		}

		void Play(MapType aMapValue, AnimationFlag aFlag)
		{

		}

		void Update();

		void Stop()
		{
			if (myAnimator == nullptr)
				return;

			myAnimator->myAnimationState = AnimationState::eEnded;
		}

		bool IsPlaying() 
		{
			if (myAnimator == nullptr)
				return false;

			return myAnimator->myAnimationState == AnimationState::ePlaying; 
		}
		  

		MapType GetCurrentMap() { return myCurrentMapValue; }
		MapType GetLastMap() { return myLastMapValue; }
		AnimatorComponent* GetAnimatorComp() { return myAnimator; }

	private:	//data
		EntityID myEntity = EntityID(-1);
		AnimatorComponent* myAnimator = nullptr;
		AnimationBlendComponent* myAnimBlend = nullptr;

		AnimationBlendBehaviour myBlends[(int)OperationType::eCount];


		std::unordered_map<MapType, AnimationBehaviour> myMappedAnimations;
		LocalCountDown myLocalCountDown;


		AnimationFlag myCurrentFlag;
		MapType myCurrentMapValue = MapType();
		MapType myLastMapValue = MapType();

		bool myAnimationQued;
		MapType myQuedAnimation;
	};

	//template<typename MapType>
	//inline void AnimationHelper<MapType>::BlendSetFetchOp(MapType aMapType)
	//{
	//	if (myAnimBlend == nullptr || myAnimator == nullptr] || aMapType == cMap)
	//		return;
	//
	//	if (myBlends[(int)OperationType::eFetch].myEnabled)
	//	{
	//		AnimationBehaviour cB = myMappedAnimations[cMap];
	//		AnimationBehaviour nB = myMappedAnimations[aMapType]; 
	//
	//		if (cB.myOverrideValue <= nB.myAnimationValue)
	//			ForceBlendSetFetchOp(aMapType);
	//		return;
	//	}
	//
	//	ForceBlendSetFetchOp(aMapType);
	//}

	//template<typename MapType>
	//inline void AnimationHelper<MapType>::ForceBlendSetFetchOp(MapType aMapType)
	//{
	//	myBlends[(int)OperationType::eFetch].myMappedAnimation = aMapType;
	//	myBlends[(int)OperationType::eFetch].myEnabled = true; 
	//
	//	RecalculateBlends();
	//}

	//template<typename MapType>
	//inline void AnimationHelper<MapType>::RecalculateBlends()
	//{
	//	AnimationBlendData& aBD = myAnimBlend->myAnimationBlendData;
	//	AnimationBlendBehaviour& fetch = myBlends[(int)OperationType::eFetch];
	//	AnimationBlendBehaviour& lerp = myBlends[(int)OperationType::eFetch];
	//	AnimationBlendBehaviour& partial = myBlends[(int)OperationType::eFetch];
	//
	//
	//	AnimationBlendNode currOutNode;
	//	if (fetch.myEnabled)
	//	{
	//		size_t fetchIdx = aBD.myFetchSize;
	//		if(!fetch.myHasUpdated)
	//			aBD.myFetchOperations[fetchIdx].myTimer = 0;
	//		aBD.myFetchOperations[fetchIdx].myAnimation = myMappedAnimations[fetch.myMappedAnimation].myAnimation;
	//
	//		aBD.myFetchSize++;
	//
	//		currOutNode.myDataIndex = fetchIdx;
	//		currOutNode.myDataIndex = AnimBlendOperationType::eFetchAnimation;
	//	}
	//	if (lerp.myEnabled)
	//	{
	//		size_t fetchIdx = aBD.myFetchSize;
	//		size_t lerpIdx = aBD.myInterpSize;
	//
	//		if (!lerp.myHasUpdated)
	//			aBD.myFetchOperations[fetchIdx].myTimer = 0;
	//		aBD.myFetchOperations[fetchIdx].myAnimation = myMappedAnimations[lerp.myMappedAnimation].myAnimation;
	//
	//		aBD.myInterpOperations[lerpIdx].myNodes[0]				= currOutNode;
	//		aBD.myInterpOperations[lerpIdx].myNodes[1].myDataIndex	= fetchIdx;
	//		aBD.myInterpOperations[lerpIdx].myNodes[1].myType		= AnimBlendOperationType::eFetchAnimation;
	//		aBD.myInterpOperations[lerpIdx].myInterpID.myDataIndex	= 0;
	//		aBD.myInterpOperations[lerpIdx].myInterpID.myType		= InterpolationType::eLinear;
	//		
	//		myAnimBlend->myAnimationBlendData.myInterpLinear[0].mySpeed = lerp.myInterpSpeed;
	//		myAnimBlend->myAnimationBlendData.myInterpLinear[0].myTimer = 0; 
	//
	//		aBD.myFetchSize++;
	//		aBD.myInterpSize++;
	//
	//		currOutNode.myDataIndex = lerpIdx;
	//		currOutNode.myDataIndex = AnimBlendOperationType::eInterpolate;
	//	}
	//	if (partial.myEnabled)
	//	{
	//		size_t fetchIdx = aBD.myFetchSize;
	//		size_t partIdx = aBD.myPartialSize; 
	//
	//		if (!partial.myHasUpdated)
	//			aBD.myFetchOperations[fetchIdx].myTimer = 0;
	//		aBD.myFetchOperations[fetchIdx].myAnimation = myMappedAnimations[partial.myMappedAnimation].myAnimation;
	//
	//		aBD.myPartialOperationData[partIdx].myDeviderID			= partial.myJointID;
	//		aBD.myPartialOperationData[partIdx].myPartialInfluence	= partial.myPartialInfl;
	//
	//		aBD.myPartialOperationData[partIdx].myNodes[0]				= currOutNode;
	//		aBD.myPartialOperationData[partIdx].myNodes[1].myDataIndex	= fetchIdx;
	//		aBD.myPartialOperationData[partIdx].myNodes[1].myType		= AnimBlendOperationType::eFetchAnimation;
	//	
	//		aBD.myFetchSize++;
	//		aBD.myPartialSize++;
	//
	//		currOutNode.myDataIndex = partIdx;
	//		currOutNode.myDataIndex = AnimBlendOperationType::ePartial;
	//	}
	//
	//	myAnimBlend->myAnimationBlendData.myRootNode = currOutNode;
	//}

	template<typename MapType>
	inline void AnimationHelper<MapType>::ForcePlay(MapType aMapValue) 
	{
		if (myAnimator == nullptr)
			return;
		AnimationBehaviour& aB = myMappedAnimations[aMapValue];
		myAnimator->myTimer = 0;
		myAnimator->mySpeedModifier = aB.mySpeed;
		myAnimator->myCurrentAnimation = aB.myAnimation;
		myAnimator->myAnimationState = AnimationState::ePlaying;
		myAnimator->myLoopingFlag = (aB.myAnimationFlag & AH_LOOPING) > 0;

		myCurrentFlag = aB.myAnimationFlag;
		myLastMapValue = myCurrentMapValue;
		myCurrentMapValue = aMapValue;

		if (myAnimBlend != nullptr)
		{
			myAnimator->myTimer = myAnimBlend->myAnimationBlendData.myFetchOperations[0].myTimer;
			Context::Get()->mySceneHandler->GetActiveScene().RemoveComponent<AnimationBlendComponent>(myEntity);
		}
	}

	template<typename MapType>
	inline void ISTE::AnimationHelper<MapType>::ForcePlayInterpelated(MapType aFrom, MapType aTo, float anInterpSpeed)
	{
		AnimationBehaviour& from = myMappedAnimations[aFrom];
		AnimationBehaviour& to	 = myMappedAnimations[aTo];
		myLastMapValue = aFrom;
		myCurrentMapValue = aTo;

		myAnimator->myAnimationState = AnimationState::ePlaying; 

		if (myAnimBlend == nullptr)
			myAnimBlend = Context::Get()->mySceneHandler->GetActiveScene().AssignComponent<AnimationBlendComponent>(myEntity);
		else 
			myAnimBlend->myAnimationBlendData = {}; 
		
		{//setting up for interpolation
		myAnimBlend->myAnimationBlendData.myRootNode.myType = AnimBlendOperationType::eInterpolate;
		myAnimBlend->myAnimationBlendData.myRootNode.myDataIndex = 0;

		myAnimBlend->myAnimationBlendData.myFetchSize		= 2;
		myAnimBlend->myAnimationBlendData.myInterpSize		= 1;
		myAnimBlend->myAnimationBlendData.myInterpLinSize	= 1;
		

		myAnimBlend->myAnimationBlendData.myFetchOperations[0].myAnimation = from.myAnimation;
		myAnimBlend->myAnimationBlendData.myFetchOperations[0].myTimer = myAnimator->myTimer;

		myAnimBlend->myAnimationBlendData.myFetchOperations[1].myAnimation = to.myAnimation;
		myAnimBlend->myAnimationBlendData.myFetchOperations[1].myTimer = 0.f;

		myAnimBlend->myAnimationBlendData.myInterpOperations[0].myInterpID.myDataIndex = 0;
		myAnimBlend->myAnimationBlendData.myInterpOperations[0].myInterpID.myType = InterpolationType::eLinear;
		
		myAnimBlend->myAnimationBlendData.myInterpOperations[0].myNodes[0].myType = AnimBlendOperationType::eFetchAnimation;
		myAnimBlend->myAnimationBlendData.myInterpOperations[0].myNodes[0].myDataIndex = 0; 
		myAnimBlend->myAnimationBlendData.myInterpOperations[0].myNodes[1].myType		= AnimBlendOperationType::eFetchAnimation;
		myAnimBlend->myAnimationBlendData.myInterpOperations[0].myNodes[1].myDataIndex	= 1;

		myAnimBlend->myAnimationBlendData.myInterpLinear[0].mySpeed = anInterpSpeed;
		myAnimBlend->myAnimationBlendData.myInterpLinear[0].myTimer = 0;
		} 
		EntityID entity = myEntity;
		myLocalCountDown.SetRepeat(false);
		myLocalCountDown.SetDuration(1.f / anInterpSpeed); 
		myLocalCountDown.SetCallback([this, aTo]() {
			ForcePlay(aTo);
			myAnimator->myTimer = myAnimBlend->myAnimationBlendData.myFetchOperations[1].myTimer;
			Context::Get()->mySceneHandler->GetActiveScene().RemoveComponent<AnimationBlendComponent>(myEntity);
			myAnimBlend = nullptr; 
		}); 
		myLocalCountDown.SetOn(true);
	}


	template<typename MapType>
	inline void ISTE::AnimationHelper<MapType>::PlayInterpelated(MapType aMapValue, float anInterpSpeed, MapType aFromMapValue)
	{
		if ((int)aFromMapValue == -1)
		{
			aFromMapValue = myCurrentMapValue;
		}
		
		if (aMapValue == myCurrentMapValue || myAnimator == nullptr || myCurrentMapValue == aMapValue)
			return;

		if (myAnimator->myAnimationState == AnimationState::ePlaying)
		{

			AnimationBehaviour& cB = myMappedAnimations[myCurrentMapValue];
			AnimationBehaviour& nB = myMappedAnimations[aMapValue];

			if (cB.myOverrideValue <= nB.myAnimationValue)
				ForcePlayInterpelated(aFromMapValue, aMapValue, anInterpSpeed);

			return;
		}
		ForcePlayInterpelated(aFromMapValue, aMapValue, anInterpSpeed);
	}

	template<typename MapType>
	inline void AnimationHelper<MapType>::ForcePlayPartially(MapType aMapValue, MapType aMapToBlendWith, int aJointIndex, float aPartialStr, bool aFlipped, bool aShouldClear)
	{
		AnimationBehaviour& from = myMappedAnimations[aMapValue];
		AnimationBehaviour& to = myMappedAnimations[aMapToBlendWith];
		myLastMapValue = aMapValue;
		myCurrentMapValue = aMapToBlendWith;

		if (myAnimBlend == nullptr)
			myAnimBlend = Context::Get()->mySceneHandler->GetActiveScene().AssignComponent<AnimationBlendComponent>(myEntity);
		else if (aShouldClear)
			myAnimBlend->myAnimationBlendData = {};
		else 
		{
			myAnimBlend->myAnimationBlendData.myFetchOperations[0].myAnimation = 0;
		}
		myAnimator->myAnimationState = AnimationState::ePlaying; 
		
		{//setting up for partiallity
		myAnimBlend->myAnimationBlendData.myRootNode.myType = AnimBlendOperationType::ePartial;
		myAnimBlend->myAnimationBlendData.myRootNode.myDataIndex = 0;

		myAnimBlend->myAnimationBlendData.myFetchSize		= 2;
		myAnimBlend->myAnimationBlendData.myPartialSize		= 1;

		myAnimBlend->myAnimationBlendData.myFetchOperations[0].myAnimation = from.myAnimation;
		myAnimBlend->myAnimationBlendData.myFetchOperations[0].myTimer;

		myAnimBlend->myAnimationBlendData.myFetchOperations[1].myAnimation = to.myAnimation;
		myAnimBlend->myAnimationBlendData.myFetchOperations[1].myTimer;
		
		myAnimBlend->myAnimationBlendData.myPartialOperationData[0].myDeviderID = aJointIndex;
		myAnimBlend->myAnimationBlendData.myPartialOperationData[0].myPartialInfluence = aPartialStr;
		myAnimBlend->myAnimationBlendData.myPartialOperationData[0].myNodes[0].myType = AnimBlendOperationType::eFetchAnimation;
		myAnimBlend->myAnimationBlendData.myPartialOperationData[0].myNodes[0].myDataIndex = 0;
		myAnimBlend->myAnimationBlendData.myPartialOperationData[0].myNodes[1].myType		= AnimBlendOperationType::eFetchAnimation;
		myAnimBlend->myAnimationBlendData.myPartialOperationData[0].myNodes[1].myDataIndex	= 1; 
		}

		if ((to.myAnimationFlag & AH_LOOPING) != AH_LOOPING)
		{
			EntityID entity = myEntity;
			Animation* anim = Context::Get()->myAnimationManager->GetAnimation(myAnimBlend->myAnimationBlendData.myFetchOperations[(int)aFlipped].myAnimation);
			float duration = anim->myLengthInSeconds - myAnimBlend->myAnimationBlendData.myFetchOperations[(int)aFlipped].myTimer;
			
			myLocalCountDown.SetRepeat(false);
			myLocalCountDown.SetDuration(duration);
			myLocalCountDown.SetOn(true);
			myLocalCountDown.SetCallback([this, aMapValue, aFlipped]() {
				ForcePlay(aMapValue);
				myAnimator->myTimer = myAnimBlend->myAnimationBlendData.myFetchOperations[(int)aFlipped].myTimer;
				Context::Get()->mySceneHandler->GetActiveScene().RemoveComponent<AnimationBlendComponent>(myEntity);
				myAnimBlend = nullptr;
				});
		}
	}

	template<typename MapType>
	inline void AnimationHelper<MapType>::PlayPartially(MapType aMapValue, MapType aMapToBlendWith, int aBoneIndex, float aPartialStr, bool aFlipped, bool aShouldClear)
	{
		if (myAnimator == nullptr || aMapValue == aMapToBlendWith)
			return; 

		if (myAnimator->myAnimationState == AnimationState::ePlaying)
		{

			AnimationBehaviour& cB = myMappedAnimations[aMapToBlendWith];
			AnimationBehaviour& nB = myMappedAnimations[aMapToBlendWith];

			if (cB.myOverrideValue <= nB.myAnimationValue)
				ForcePlayPartially(aMapValue, aMapToBlendWith, aBoneIndex, aPartialStr, aFlipped, aShouldClear);

			return;
		}
		ForcePlayPartially(aMapValue, aMapToBlendWith, aBoneIndex, aPartialStr, aFlipped, aShouldClear);
	}
 
	template<typename MapType>
	inline void ISTE::AnimationHelper<MapType>::Play(MapType aMapValue)
	{
		if ( aMapValue == myCurrentMapValue || myAnimator == nullptr)
			return;

		if (myAnimator->myAnimationState == AnimationState::ePlaying)
		{

			AnimationBehaviour& cB = myMappedAnimations[myCurrentMapValue];
			AnimationBehaviour& nB = myMappedAnimations[aMapValue];

			if (cB.myOverrideValue <= nB.myAnimationValue)
				ForcePlay(aMapValue);

			return;
		}
		ForcePlay(aMapValue);
	}
	template<typename MapType>
	inline void AnimationHelper<MapType>::Update()
	{  
		myLocalCountDown.Update(Context::Get()->myTimeHandler->GetDeltaTime());
		//empty
	}
}