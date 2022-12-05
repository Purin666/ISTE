#pragma once

#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/Math/Vec.h"
#include "ISTE/VFX/VFXCommonDefines.h"

#include <string>	
#include <vector>
#include <unordered_map>
#include <map>

namespace ISTE
{
	using Color = CU::Vec4f;

	struct ModelVFXData
	{
		// Model
		std::string	myModelPath = "Default";

		// Textures
		std::vector<std::string>myTexturePath;
		std::vector<CU::Vec3f>	mySample;
		std::vector<CU::Vec2f>	myUVOverLife;

		// Generic
		FloatDist				myLifetime;
		FloatDist				myDelay;
		FloatDist				myFirstDelay;
		bool					myShouldLoop;
		
		// Color
		CU::Vec3f				myInitialColor;
		std::vector<CU::Vec3f>	myColorOverLife;

		// Size
		Vec3fDist				myInitialSize;
		std::vector<CU::Vec3f>	mySizeOverLife;

		// Rotation
		Vec3fDist				myInitialRotation;
		std::vector<CU::Vec3f>	myRotationRateOverLife;

		// Spawn Location
		Vec3fDist				myInitialBoxPosition;
		FloatDist				myInitialSpherePosition;

		// Physics
		Vec3fDist				myInitialVelocity;
		Vec3fDist				myConstAcceleration;
		FloatDist				myMass;
		FloatDist				myResistance;
	};

	struct ModelVFX
	{
		EntityID myEntityID = INVALID_ENTITY;
		EntityID myAttachedID = INVALID_ENTITY;

		std::string myTypeName;

		float myRelativeTime = 0.f;
		float myLifetime;
		float myDelay;
		float myFirstDelay;
		bool myShouldLoop;

		CU::Vec3f myColor;
		CU::Vec3f myPosition;
		CU::Vec3f mySize;
		CU::Vec3f myRotation;

		CU::Vec3f myVelocity;
		CU::Vec3f myConstAcceleration;
		float myMass;
		float myResistance;
	};

	struct ModelVFXSystem
	{
		std::map<std::string, ModelVFXData> myDatas;
		std::unordered_map<EntityID, ModelVFX> myVFX;
	};
}