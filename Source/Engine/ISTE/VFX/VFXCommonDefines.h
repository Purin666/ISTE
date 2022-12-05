#pragma once

#include "ISTE/Math/Vec.h"
#include "ISTE/Math/Math.h"

#define MAX_PARTICLES_PER_EMITTER 128

namespace ISTE
{
	using Color = CU::Vec4f;

	struct TextureRect // We dont have a struct encapsulating this in the sprite
	{
		float myStartX;
		float myStartY;
		float myEndX;
		float myEndY;
	};

	struct FloatDist
	{
		float myMin = 1.f;
		float myMax = 1.f;
	};
	struct Vec2fDist
	{
		CU::Vec2f myMin;
		CU::Vec2f myMax;
	};
	struct Vec3fDist
	{
		CU::Vec3f myMin;
		CU::Vec3f myMax;
	};
	struct IntDist
	{
		int myMin = 1;
		int myMax = 1;
	};
}