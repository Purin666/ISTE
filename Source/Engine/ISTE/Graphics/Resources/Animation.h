#pragma once 
#include <vector>
#include <ISTE/Math/Vec3.h>
#include <ISTE/Math/Quaternion.h>
#include <ISTE/Graphics/RenderDefines.h> 
#include <ISTE/Graphics/ComponentAndSystem/TransformComponent.h>
namespace ISTE {
	enum class AnimationState {
		ePaused,
		ePlaying,
		eEnded,

		eCount
	};

	struct Animation {
		struct KeyFrame {
			TransformComponent myTransforms[MAX_BONE_COUNT];
		};
		std::vector<KeyFrame> myKeyFrames;
		int myKeyFrameCount;

		float myFps = 0.f;
		float myLengthInFrames = 0.f;
		float myLengthInSeconds = 0.f;
		ModelID myModel = ModelID(-1);
		AnimationID myIndex = AnimationID(-1);
		std::string myAnimationName;
		std::string myPath;
	};
};