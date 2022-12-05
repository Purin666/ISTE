#pragma once

#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/Graphics/RenderDefines.h" // TextureID

#include "ISTE/VFX/VFXCommonDefines.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <map>

#define MAX_PARTICLES_PER_EMITTER 128

namespace ISTE
{
	struct Emitter3DData
	{
		std::string				myTexturePath;

		IntDist					mySubImages; // min = horizontal, max = vertical
		float					mySubImagesPerSecond = 1.f;
		bool					myPickRandomSubImage = false;

		FloatDist				myEmitterDuration;
		FloatDist				myLifeTime;
		FloatDist				myInitialDelay;
		FloatDist				mySpawnRate;
		IntDist					myInitialSpawnAmount;
		IntDist					mySpawnAmount;
		CU::Vec3f 				myFaceDirection = CU::Vec3f(0, 1, 0);
		bool  					myFaceTowardsCamera = true;
		bool					myUseLocalSpace = false;

		CU::Vec3f				myInitialColor = CU::Vec3f(1, 1, 1);
		std::vector<CU::Vec3f>	myColorOverLife;

		float					myInitialAlpha = 1.f;
		std::vector<float>		myAlphaOverLife;

		Vec3fDist				myInitialSize;
		std::vector<CU::Vec3f>	mySizeOverLife;

		FloatDist				myInitialRotation;
		FloatDist				myRotationRate;

		Vec3fDist				myInitialBoxPosition;
		FloatDist				myInitialSpherePosition;
		bool					myClampWithinBox = false;

		Vec3fDist				myInitialVelocity;
		Vec3fDist				myConstAcceleration;
		FloatDist				myMass;
		FloatDist				myResistance;

		TextureID myTextureID;	// not serialized
	};

	struct Particle3D
	{
		int			myId;

		float		myRelativeTime;
		float		myLifetime;
		float		myRotation;
		float		myRotationRate;
		float		myMass;
		float		myResistance;

		CU::Vec3f	myFaceDirection;
		CU::Vec3f	myPosition;
		CU::Vec3f	mySize;
		CU::Vec3f	myVelocity;
		CU::Vec3f	myConstAcceleration;
	};

	struct Emitter3D
	{
		std::string myTypeName;

		CU::Vec3f	myPosition;
		CU::Vec3f	myEulerAngles;

		float		myEmitterTime;
		float		myTimeUntilNextParticle;

		bool		myIsDisabled;							// a disabled emitter will automatically be destroyed once all its particles are dead

		std::vector<Particle3D> myParticles;
	};

	struct Emitter3DSystem
	{
		int myLatestParticleId = 0;
		int myLatestEmitterId = 0;

		std::map<std::string, Emitter3DData> myEmitterDatas;
		std::unordered_map<EntityID, Emitter3D>		 myEmitters;
	};
}