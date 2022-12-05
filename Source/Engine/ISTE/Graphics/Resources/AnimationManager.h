#pragma once
#include <string>

#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/Graphics/RenderDefines.h"

#include "ISTE/Graphics/Resources/FBXLoader.h"
#include "ISTE/Graphics/Resources/AnimationDefines.h"
#include "ISTE/Graphics/Resources/Animation.h"

#include "ISTE/CU/HashMap.hpp"

namespace ISTE
{
	struct Context;

	class AnimationManager
	{ 
	public:
		~AnimationManager()
		{
			myAnimations.ClearFromHeap(); 
		}

		bool Init();

		/*
		* Loads an animation and assigns it to a model
		* Handling the AnimatorComponent must be done manually
		* @param aModelIndex: The model to assign the animation to
		* @param anAnimation: The filepath of the animation to load
		*/
		AnimationLoadResult LoadAnimation(ModelID aModelIndex, const std::string& anAnimation);

		/*
		* Loads an animation and assigns it to the model currently assigned to the entity
		* Assumes entity has a ModelComponent
		* @param aEntityID: The Entity to assign the animation to
		* @param anAnimation: The filepath of the animation to load
		*/
		AnimationLoadResult LoadAnimation(EntityID aEntityID, const std::string& anAnimation);

		 /*
		 
		 
		 */
		AnimationBlendID LoadAnimationBlend(EntityID aEntityID, const std::string& path);

		bool SaveAnimationBlend(const AnimationBlendData&, const std::string& path);

		AnimationBlendData& GetAnimationBlend(AnimationBlendID);


		/*
			Gets an animation by its index
		*/
		Animation* GetAnimation(AnimationID aAnimIndex) { return myAnimations.GetByIterator(aAnimIndex); }
		CU::HashMap<std::string, Animation*, MAX_CACHED_ANIMATIONS_COUNT, AnimationID>& GetAnimationList() { return myAnimations; }
		
	private:
		CU::HashMap<std::string, Animation*,			MAX_CACHED_ANIMATIONS_COUNT, AnimationID>		myAnimations;
		CU::HashMap<std::string, AnimationBlendData,	MAX_CACHED_ANIMATIONS_COUNT, AnimationBlendID>	myBlendData;
		FBXLoader myFbxLoader;
		Context* myCtx;
	};

}
