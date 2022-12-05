#pragma once

#include <math.h>

#include "../Math/Vec3.h"

namespace ISTE // Most Functions are from http://totologic.blogspot.com/2014/01/accurate-point-in-triangle-test.html
{
	//OBS
	// y is always the same
	// Usage x and z.

	constexpr float Epsilon = 0.001f;
	constexpr float EpsilonSquare = Epsilon * Epsilon;

	static const float NAVSide(CU::Vec3f aPoint1, CU::Vec3f aPoint2, CU::Vec3f aCheck)
	{
		return (aPoint2.z - aPoint1.z) * (aCheck.x - aPoint1.x) + (-aPoint2.x + aPoint1.x) * (aCheck.z - aPoint1.z);
	}

	static const bool NAVNaivePointInTriangle(CU::Vec3f aPoint1, CU::Vec3f aPoint2, CU::Vec3f aPoint3, CU::Vec3f aCheck)
	{
		bool checkSide1 = NAVSide(aPoint1, aPoint2, aCheck) >= 0;
		bool checkSide2 = NAVSide(aPoint2, aPoint3, aCheck) >= 0;
		bool checkSide3 = NAVSide(aPoint3, aPoint1, aCheck) >= 0;
		return checkSide1 && checkSide2 && checkSide3;
	}

	static const bool NAVPointInTriangleBoundingBox(CU::Vec3f aPoint1, CU::Vec3f aPoint2, CU::Vec3f aPoint3, CU::Vec3f aCheck)
	{
		float xMin = std::fmin(aPoint1.x, std::fmin(aPoint2.x, aPoint3.x)) - Epsilon;
		float xMax = std::fmax(aPoint1.x, std::fmax(aPoint2.x, aPoint3.x)) + Epsilon;
		float zMin = std::fmin(aPoint1.z, std::fmin(aPoint2.z, aPoint3.z)) - Epsilon;
		float zMax = std::fmax(aPoint1.z, std::fmax(aPoint2.z, aPoint3.z)) + Epsilon;

		if (aCheck.x < xMin || aCheck.x > xMax || aCheck.z < zMin || aCheck.z > zMax)
			return false;
		else
			return true;
	}

	static const bool NAVPointInTriangle(CU::Vec3f aPoint1, CU::Vec3f aPoint2, CU::Vec3f aPoint3, CU::Vec3f aCheck)
	{
		float denominator = (aPoint1.x * (aPoint2.z - aPoint3.z) + aPoint1.z * (aPoint3.x - aPoint2.x) + aPoint2.x * aPoint3.z - aPoint2.z * aPoint3.x);
		float t1 = (aCheck.x * (aPoint3.z - aPoint1.z) + aCheck.z * (aPoint1.x - aPoint3.x) - aPoint1.x * aPoint3.z + aPoint1.z * aPoint3.x) / denominator;
		float t2 = (aCheck.x * (aPoint2.z - aPoint1.z) + aCheck.z * (aPoint1.x - aPoint2.x) - aPoint1.x * aPoint2.z + aPoint1.z * aPoint2.x) / -denominator;
		float s = t1 + t2;

		return (0 <= t1 && t1 <= 1 && 0 <= t2 && t2 <= 1 && s <= 1);
	}

	static const float NAVDistanceSquarePointToSegment(CU::Vec3f aPoint1, CU::Vec3f aPoint2, CU::Vec3f aCheck)
	{
		float p1p2SquareLength = std::powf((aPoint2.x - aPoint1.x), 2) + std::powf((aPoint2.z - aPoint1.z), 2);
		float dotProduct = ((aCheck.x - aPoint1.x) * (aPoint2.x - aPoint1.x) + (aCheck.z - aPoint1.z) * (aPoint2.z - aPoint1.z)) / p1p2SquareLength;

		if (dotProduct < 0)
		{
			return std::powf((aCheck.x - aPoint1.x), 2) + std::powf((aCheck.z - aPoint1.z), 2);
		}
		else if (dotProduct <= 1)
		{
			float checkp1SquareLength = std::powf((aPoint1.x - aCheck.x), 2) + std::powf((aPoint1.z - aCheck.z), 2);
			return checkp1SquareLength - dotProduct * dotProduct * p1p2SquareLength;
		}
		else
		{
			return std::powf((aCheck.x - aPoint2.x), 2) + std::powf((aCheck.z - aPoint2.z), 2);
		}
	}

	static const bool NAVAccuratePointInTriangle(CU::Vec3f aPoint1, CU::Vec3f aPoint2, CU::Vec3f aPoint3, CU::Vec3f aCheck) // Call this (:
	{
		if (!NAVPointInTriangleBoundingBox(aPoint1, aPoint2, aPoint3, aCheck))
			return false;

		if (NAVNaivePointInTriangle(aPoint1, aPoint2, aPoint3, aCheck))
			return true;

		if (NAVDistanceSquarePointToSegment(aPoint1, aPoint2, aCheck) <= EpsilonSquare)
			return true;
		if (NAVDistanceSquarePointToSegment(aPoint2, aPoint3, aCheck) <= EpsilonSquare)
			return true;
		if (NAVDistanceSquarePointToSegment(aPoint3, aPoint1, aCheck) <= EpsilonSquare)
			return true;

		return false;
	}
}