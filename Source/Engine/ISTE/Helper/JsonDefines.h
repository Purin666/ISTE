#pragma once

#include "Json/json.hpp"

#include "ISTE/Math/Vec.h"

#include "ISTE/VFX/SpriteParticles/SharedSpriteParticleDefines.h"
#include "ISTE/VFX/VFXSystemDefines.h"
#include "ISTE/VFX/ModelVFX/ModelVFXDefines.h"
#include "ISTE/VFX/SpriteParticles/Sprite2DParticleDefines.h"
#include "ISTE/VFX/SpriteParticles/Sprite3DParticleDefines.h"
#include "ISTE/UI/UIDefines.h"


namespace CU
{
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vec2f, x, y)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vec3f, x, y, z)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ISTE::Color, r, g, b, a)
}
namespace ISTE
{
	// vfx & particles
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(IntDist, myMin, myMax)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(FloatDist, myMin, myMax)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vec2fDist, myMin, myMax)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Vec3fDist, myMin, myMax)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(ModelVFXData, myModelPath, myTexturePath, mySample, myUVOverLife, myLifetime, myDelay, myFirstDelay, myShouldLoop, myInitialColor, myColorOverLife, myInitialSize, mySizeOverLife, myInitialRotation, myRotationRateOverLife, myInitialBoxPosition, myInitialSpherePosition, myInitialVelocity, myConstAcceleration, myMass, myResistance)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Emitter2DData,
		myTexturePath,
		mySubImages,
		myPickRandomSubImage,
		mySubImagesPerSecond,
		myEmitterDuration,
		mySpawnRate,
		myInitialDelay,
		myInitialSpawnAmount,
		mySpawnAmount,
		myUseLocalSpace,
		myLifeTime,
		myInitialColor,
		myColorOverLife,
		myInitialAlpha,
		myAlphaOverLife,
		myInitialSize,
		mySizeOverLife,
		myInitialRotation,
		myRotationRate,
		myInitialBoxPosition,
		myInitialCirclePosition,
		myClampWithinBox,
		myInitialVelocity,
		myConstAcceleration,
		myMass,
		myResistance
		)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Emitter3DData,
		myTexturePath,
		mySubImages,
		myPickRandomSubImage,
		mySubImagesPerSecond,
		myEmitterDuration,
		myInitialDelay,
		mySpawnRate,
		myInitialSpawnAmount,
		mySpawnAmount,
		myUseLocalSpace,
		myFaceTowardsCamera,
		myFaceDirection,
		myLifeTime,
		myInitialColor,
		myColorOverLife,
		myInitialAlpha,
		myAlphaOverLife,
		myInitialSize,
		mySizeOverLife,
		myInitialRotation,
		myRotationRate,
		myInitialBoxPosition,
		myInitialSpherePosition,
		myClampWithinBox,
		myInitialVelocity,
		myConstAcceleration,
		myMass,
		myResistance)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(VFXData, myEmitterTypes, myModelVFXTypes, myLifetime, myDelay, myFirstDelay, myShouldLoop)
	// ui
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Sprite2DRenderCommand,
		myPosition,
		myScale,
		myRotation,
		myPivot,
		myColor,
		myUVStart,
		myUVEnd,
		myUVScale,
		myUVOffset)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(TextRenderCommand,
		myText,
		myPivot,
		myColor,
		myPosition,
		myRotation,
		myTextScale/*,
		myLetterSpacing,
		myWordSpacing,
		myLineSpacing*/)
	
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Image, myCommand, myDefaultColor, myHightlightColor, myClickColor, myPath)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Text, myCommand, myPath, mySize, myBorder)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Button, myImage, myHitboxScale, myCallbackName)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(GameplayImage, myImage, myHitboxScale, myType)
	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Tooltip, myImages, myTexts)

	NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(UI, myImages, myTexts, myButtons, myGameplayImages, myTooltips)
}
