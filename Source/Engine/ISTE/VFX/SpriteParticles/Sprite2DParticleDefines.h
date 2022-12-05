#pragma once

#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/Graphics/RenderDefines.h" // TextureID

#include "ISTE/VFX/VFXCommonDefines.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <map>

namespace ISTE
{
	struct Emitter2DData
	{
		std::string				myTexturePath;

		IntDist					mySubImages; // min = horizontal, max = vertical
		bool					myPickRandomSubImage = false;
		float					mySubImagesPerSecond = 1.f;

		FloatDist				myEmitterDuration;
		FloatDist				myLifeTime;
		FloatDist				myInitialDelay;
		FloatDist				mySpawnRate;
		IntDist					myInitialSpawnAmount;
		IntDist					mySpawnAmount;
		bool					myUseLocalSpace = false;

		CU::Vec3f				myInitialColor = CU::Vec3f(1, 1, 1);
		std::vector<CU::Vec3f>	myColorOverLife;

		float					myInitialAlpha = 1.f;
		std::vector<float>		myAlphaOverLife;

		Vec2fDist				myInitialSize;
		std::vector<CU::Vec2f>	mySizeOverLife;

		FloatDist				myInitialRotation;
		FloatDist				myRotationRate;

		Vec2fDist				myInitialBoxPosition;
		FloatDist				myInitialCirclePosition;
		bool					myClampWithinBox = false;

		Vec2fDist				myInitialVelocity;
		Vec2fDist				myConstAcceleration;
		FloatDist				myMass;
		FloatDist				myResistance;


		TextureID myTextureID;
	};

	struct Particle2D
	{
		int			myId;

		float		myRelativeTime;
		float		myLifetime;
		float		myRotation;
		float		myRotationRate;
		float		myMass;
		float		myResistance;

		CU::Vec2f	myPosition;
		CU::Vec2f	mySize;
		CU::Vec2f	myVelocity;
		CU::Vec2f	myConstAcceleration;
	};

	struct Emitter2D
	{
		std::string myTypeName;

		CU::Vec2f	myPosition;
		CU::Vec2f	myAngle;

		float		myEmitterTime;
		float		myTimeUntilNextParticle;

		bool		myIsDisabled;							// a disabled emitter will automatically be destroyed once all its particles are dead

		std::vector<Particle2D> myParticles;
	};

	struct Emitter2DSystem
	{
		int myLatestParticleId = 0;
		int myLatestEmitterId = 0;

		std::map<std::string, Emitter2DData> myEmitterDatas;
		std::unordered_map<EntityID, Emitter2D>		 myEmitters;
	};
}