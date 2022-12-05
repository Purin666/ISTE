#pragma once
#include "ISTE/Graphics/RenderDefines.h"
#include "ISTE/Math/Matrix4x4.h"
#include "ISTE/Context.h"
#include "ISTE/Time/TimeHandler.h"

#define MAX_ANIM_BLEND_OPERATIONS 10
#define ANIM_BLEND_TREE_SIZE 20

namespace ISTE {


	enum class InterpolationType
	{
		eConstant,
		eLinear,
		eSin,
		eCount
		//mwba
	};

	struct InterpolationID {
		InterpolationType	myType;
		size_t				myDataIndex;
	};

	namespace InterpolationData {

		struct ConstantData {
			float myTValue = 0;
			
			float myTimer = 0;
		};
		//f(x) = x * mySpeed
		struct LinearData {
			float mySpeed = 1;		//X ( R 
			float myTimer = 0;
		};

		//f(x) = (0.5 + sin(x*mySpeed) * 0.5) * myModifier;
		struct SinData {
			float mySpeed = 1;		//X ( R
			float myModifier = 1;	//0 >= X <= 1 
			
			float myTimer = 0;
		};
	};

	enum class AnimBlendOperationType
	{
		eFetchAnimation,
		eInterpolate,
		ePartial,
		eCount
		//mwba
	};

	struct AnimationLoadResult {
		operator AnimationID() { return myValue; }
		operator bool() { return mySuccesFlag; }

		bool		mySuccesFlag;
		AnimationID myValue;
	};

	struct AnimationBlendLoadResult {
		operator AnimationBlendID() { return myValue; }
		operator bool() { return mySuccesFlag; }

		bool				mySuccesFlag;
		AnimationBlendID	myValue;
	};

	struct Pose {
		CU::Matrix4x4f myLocalSpaceBoneTransforms[MAX_BONE_COUNT];
		CU::Matrix4x4f myModelSpaceBoneTransforms[MAX_BONE_COUNT];
		CU::Matrix4x4f myBindSpaceTransforms[MAX_BONE_COUNT];
	};

#pragma region AnimatoinBlend Data
	struct AnimationBlendNode{
		AnimBlendOperationType	myType;
		size_t					myDataIndex;
	};
	
	
	struct FetchAnimationData {
		float		myTimer = 0;
		float		mySpeed = 1;
		AnimationID myAnimation = AnimationID(-1);
	};

	struct AnimationInterpolationData { 
		float				myLerpValue;
		AnimationBlendNode	myNodes[2];
		InterpolationID		myInterpID;
	};

	struct AnimationAddativeData {
		AnimationBlendNode	myNodes[2];
	};

	struct AnimationPartialData {
		float				myPartialInfluence;
		size_t				myDeviderID;
		AnimationBlendNode	myNodes[2];
	};
#pragma endregion

	struct AnimationBlendData {
		AnimationBlendID			myID;
		AnimationBlendNode			myRootNode;

		AnimationInterpolationData	myInterpOperations[MAX_ANIM_BLEND_OPERATIONS];
		FetchAnimationData			myFetchOperations[MAX_ANIM_BLEND_OPERATIONS];
		AnimationAddativeData		myAddativeOperation[MAX_ANIM_BLEND_OPERATIONS];
		AnimationPartialData		myPartialOperationData[MAX_ANIM_BLEND_OPERATIONS];

		size_t myInterpSize		= 0;
		size_t myFetchSize		= 0;
		size_t myAddSize		= 0;
		size_t myPartialSize	= 0;


		//moooove out this data OUT OF HERE
		InterpolationData::ConstantData		 myInterpConstant[MAX_ANIM_BLEND_OPERATIONS];
		InterpolationData::LinearData		 myInterpLinear[MAX_ANIM_BLEND_OPERATIONS];
		InterpolationData::SinData			 myInterpSin[MAX_ANIM_BLEND_OPERATIONS];
		
		size_t myInterpConstantSize = 0;
		size_t myInterpLinSize		= 0;
		size_t myInterpSinSize		= 0;
	}; 

};