#pragma once

#include "ISTE/VFX/VFXCommonDefines.h"
#include "ISTE/CU/UtilityFunctions.hpp"

namespace ISTE
{
	inline float Rand(ISTE::IntDist aDist)
	{
		return CU::GetRandomInt(aDist.myMin, aDist.myMax);
	}
	inline float Rand(ISTE::FloatDist aDist)
	{
		float t = CU::GetRandomFloat(0, 1);
		return t * aDist.myMin + (1.f - t) * aDist.myMax;
	}
	inline CU::Vec2f Rand(ISTE::Vec2fDist aDist)
	{
		CU::Vec2f t(CU::GetRandomFloat(0, 1), CU::GetRandomFloat(0, 1));
		return t * aDist.myMin + (CU::Vec2f(1.f, 1.f) - t) * aDist.myMax;
	}
	inline CU::Vec3f Rand(ISTE::Vec3fDist aDist)
	{
		CU::Vec3f t(CU::GetRandomFloat(0, 1), CU::GetRandomFloat(0, 1), CU::GetRandomFloat(0, 1));
		return t * aDist.myMin + (CU::Vec3f(1.f, 1.f, 1.f) - t) * aDist.myMax;
	}
	inline CU::Vec2f RandPointInsideUnitCircle() // uniformly
	{
		float x = CU::GetRandomFloat(0, 1) - 0.5f;
		float y = CU::GetRandomFloat(0, 1) - 0.5f;
		float r = std::cbrtf(CU::GetRandomFloat(0, 1)) / std::sqrtf(x * x + y * y);
		return { x * r, y * r };
	}
	inline CU::Vec3f RandPointInsideUnitSphere() // uniformly
	{
		float x = CU::GetRandomFloat(0, 1) - 0.5f;
		float y = CU::GetRandomFloat(0, 1) - 0.5f;
		float z = CU::GetRandomFloat(0, 1) - 0.5f;
		float r = std::cbrtf(CU::GetRandomFloat(0, 1)) / std::sqrtf(x * x + y * y + z * z);
		return { x * r, y * r, z * r };
	}
	inline int RandIntMap(int n) // don't input 0
	{
		std::minstd_rand e(n);
		e.discard(4); // should be enough and also quite fast
		return e();
	}


	inline ISTE::TextureRect CalcTextureRect(int mySubImgH, int mySubImgV, int aSubImgIndex)
	{
		const int    x = aSubImgIndex % mySubImgH;
		const int    y = (aSubImgIndex / mySubImgH) % mySubImgV;
		const float dx = 1.f / static_cast<float>(mySubImgH);
		const float dy = 1.f / static_cast<float>(mySubImgV);
		return { x * dx, y * dy, (x + 1) * dx, (y + 1) * dy };
	}


	template <typename T>
	T PiecewiseLinearOnUnitInterval(float x, T* someKnots, size_t aKnotCount)
	{
		assert(aKnotCount > 0 && "cannot compute return value since no knots are given");
		if (aKnotCount == 1) return someKnots[0];
		x = std::clamp(x, 0.f, 1.f);
		if (x == 1.f) return someKnots[aKnotCount - 1];
		const float partitions = static_cast<float>(aKnotCount - 1);
		const int   knotIndex = static_cast<int>(x * partitions); // always < partitionCount since x < 1
		const float localX = std::fmodf(x, 1.f / partitions) * partitions;
		return CU::Lerp(someKnots[knotIndex], someKnots[knotIndex + 1], CU::ParametricBlend(localX));
	}

}