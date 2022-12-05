#pragma once

#include "ISTE/VFX/SpriteParticles/Sprite3DParticleDefines.h"
#include "ISTE/VFX/ModelVFX/ModelVFXDefines.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <map>

namespace ISTE
{
	using VFXID = int;
	using EmitterID = int;
	using ModelVFXID = EntityID;

	struct VFXData
	{
		std::vector<std::string> myEmitterTypes;
		std::vector<std::string> myModelVFXTypes;

		FloatDist	myLifetime;
		FloatDist	myDelay;
		FloatDist	myFirstDelay;
		bool		myShouldLoop;

	};
	struct VFX
	{
		std::string myTypeName;

		std::vector<EmitterID> myEmitterIds;
		std::vector<ModelVFXID> myModelVFXIds;

		float	myRelativeTime = 0.f;
		float	myLifetime;
		float	myDelay;
		float	myFirstDelay;

		CU::Vec3f myPosition;
		CU::Vec3f myRotation;;

	};

	struct VFXSystem
	{
		VFXID myLatestVFXId = 0;

		std::map<std::string, VFXData> myDatas;
		std::unordered_map<VFXID, VFX> myPackages;
	};

}