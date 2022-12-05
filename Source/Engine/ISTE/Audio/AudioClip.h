#pragma once
#include "FMOD/fmod_common.h"
#include "FMOD/fmod_studio.hpp"
#include "FMOD/fmod_studio.hpp"
#include "FMOD/fmod_studio_guids.hpp"
#include <string>
#include <vector>


struct FMOD_3D_ATTRIBUTES;

namespace ISTE
{
	enum class SoundTypes
	{
		eMaster,
		eMusic,
		eSFX,
		eUI,
		eAmbiance,
		eCount
	};

	struct AudioClip
	{
		std::string myPath = "";
		FMOD::Sound* mySound;
		FMOD::Channel* myChannel;
		std::vector<FMOD::Channel*> myActiveChannels; //am lazy so wont' remove the other
		bool myPlayAfterDeath = false;
		bool myLoop = false;
	};
};