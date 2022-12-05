/**********************************
	Last Updated 2022-11-19
**********************************/
#pragma once
#include <ISTE/Math/Math.h>
#include <ISTE/Math/Vec.h>
#include <cassert>
#include <algorithm>
#include <random>
#include <ctime>
#include <sstream> // ToStringWithPrecision()

#pragma warning( push )
#pragma warning( disable : 4505 ) // unreferenced function with internal linkage has been removed

namespace CU
{
	template<typename T>
	T Min(const T& aFirst, const T& aSecond)
	{
		if (aFirst < aSecond)
			return aFirst;
		return aSecond;
	}
	
	template<typename T>
	T Max(const T& aFirst, const T& aSecond)
	{
		if (aFirst < aSecond)
			return aSecond;
		return aFirst;
	}

	template<typename T>
	T Abs(const T& aValue)
	{
		if (aValue < 0)
			return aValue * -1;
		return aValue;
	}

	template<typename T>
	T Clamp(const T& aMin, const T& aMax, const T& aValue)
	{
		//assert(aMin <= aMax && "Clamp() aMax is lower than aMin");

		if (aValue < aMin)
			return aMin;
		else if (aMax < aValue)
			return aMax;
		return aValue;
	}
	template<typename T>
	void Clamp(const T& aMin, const T& aMax, T& aValue)
	{
		if (aValue < aMin)
			aValue = aMin;
		else if (aMax < aValue)
			aValue = aMax;
	}

	template<typename T>
	T Lerp(const T& aFirst, const T& aSecond, const float& aFloat)
	{
		return (aFirst + aFloat * (aSecond - aFirst));
	//	return aFirst * (1.f - aFloat) + aSecond * aFloat;
	}

	template<typename T>
	void Swap(T& aFirst, T& aSecond)
	{
		T temp = aFirst;
		aFirst = aSecond;
		aSecond = temp;
	}

	template <typename T>
	std::string ToStringWithPrecision(const T a_value, const int n = 2)
	{
		std::ostringstream out;
		out.precision(n);
		out << std::fixed << a_value;
		return out.str();
	}

	// Fast Inverse Square Root - Quake III Algorithm https://youtu.be/p8u_k2LIZyo
	static float FastInverseSquareRoot(const float aNumber)
	{
		long i;
		float x2, y;
		const float threeHalfs = 1.5f;

		x2 = aNumber * 0.5f;
		y = aNumber;
		i = *(long*)&y;							// evil floating point bit hack
		i = 0x5f3759df - (i >> 1);				// what the fuck?
		y = *(float*)&i;
		y = y * (threeHalfs - (x2 * y * y));	// 1st iteration
	//	y = y * (threeHalfs - (x2 * y * y));	// 2nd iteration, can be removed
		return y;
	}
	// fast math routines from Doom3 SDK
	//static float FastInverseSquareRoot2(float x)
	//{
	//    float xhalf = 0.5f * x;
	//    int i = *(int*)&x;          // get bits for floating value
	//    i = 0x5f3759df - (i>>1);    // gives initial guess
	//    x = *(float*)&i;            // convert bits back to float
	//    x = x * (1.5f - xhalf*x*x); // Newton step
	//    return x;
	//}

	static unsigned int Factorial(unsigned int aValue)
	{
		if (aValue == 0)
			return 1;
		return aValue * Factorial(aValue - 1);
	}

	static float SigmoidBlend(const float t)
	{
		return 1.f / (1.f + exp(-t));
	}
	static float BezierBlend(const float t)
	{
		return t * t * (3.f - 2.f * t);
	}
	static float ParametricBlend(const float t)
	{
		return t * t / (2.f * (t * t - t) + 1.f);
	}
	static float WarfareBlend(const float t)
	{
		return (1.f / (t + 1.f)) - (sqrtf(t) * 0.5f);
	}
	/* Input 0 or 1, returns 0
	Input 0.5 returns 1; */
	static float EaseInOut(const float t)
	{
		float x = std::sinf(t * ISTE::PiF);
		x = 1.f - x * x;
		return 1.f - x * x;
	}

	static std::mt19937 rEngine(static_cast<unsigned int>(std::time(nullptr)));
	
	static float GetRandomFloat()
	{
		std::uniform_real_distribution<float> rnd(-100000.0f, 100000.0f);
		return rnd(rEngine);
	}
	static float GetRandomFloat(const float aMin, const float aMax)
	{
		std::uniform_real_distribution<float> rnd(aMin, aMax);
		return rnd(rEngine);
	}

	static int GetRandomInt()
	{
		std::uniform_int_distribution<int> rnd(-100000, 100000);
		return rnd(rEngine);
	}
	static int GetRandomInt(const int aMin, const int aMax)
	{
		std::uniform_int_distribution<int> rnd(aMin, aMax);
		return rnd(rEngine);
	}

	static unsigned int GetRandomUnsignedInt()
	{
		std::uniform_int_distribution<int> rnd(0, 100000);
		return rnd(rEngine);
	}
	static unsigned int GetRandomUnsignedInt(const unsigned int aMin, const unsigned int aMax)
	{
		std::uniform_int_distribution<int> rnd(aMin, aMax);
		return rnd(rEngine);
	}


	// vector util
	static const int VectorSign(const CU::Vec3f& aFirst, const CU::Vec3f& aSecond)
	{
		Vec3f result = aFirst.Cross(aSecond);
		if (result.y > 0.f)
			return 1;
		return -1;
	}
	static const Vec2f GetVectorFromRadians(const float aRadian)
	{
		return Vec2f(cos(aRadian), sin(aRadian));
	}
	static const Vec3f BezierLinear(const Vec3f& p0, const Vec3f& p1, const float t)
	{
		return (1 - t) * p0 + t * p1;
	}
	static const Vec3f BezierQuadratic(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const float t)
	{
		return powf(1 - t, 2) * p0 + 2 * (1 - t) * t * p1 + powf(t, 2) * p2;
	}
	static const Vec3f BezierCubic(const Vec3f& p0, const Vec3f& p1, const Vec3f& p2, const Vec3f& p3, const float t)
	{
		return powf(1 - t, 3) * p0 + 3 * powf(1 - t, 2) * t * p1 + 3 * (1 - t) * powf(t, 2) * p2 + powf(t, 3) * p3;
	}
}
#pragma warning( pop ) // 4505