#pragma once
#include <ISTE/Graphics/Resources/Animation.h>
#include <ISTE/Graphics/Resources/AnimationDefines.h>
#include <ISTE/Graphics/RenderDefines.h>


namespace ISTE {
	struct AnimatorComponent {

		AnimationState myAnimationState = AnimationState::ePlaying;
		bool myLoopingFlag = true;
		bool myShouldSkipModelAndLocalCalc = false;
		
		float myTimer = 0.f;
		float mySpeedModifier = 1.f;
		AnimationID myCurrentAnimation					=   AnimationID(-1);
		AnimationID myAnimations[MAX_ANIMATION_COUNT]	= { AnimationID(-1)};
		int myAnimationCount = 0; 
		
		
		Pose myPose;
	};
};