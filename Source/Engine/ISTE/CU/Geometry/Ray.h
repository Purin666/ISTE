#pragma once
#include "ISTE/Math/Vec3.h"

namespace CU
{
	template <class T>
	class Ray
	{
	public:
		Vec3<T> origin, direction;

		Ray() = default;
		Ray(const Ray<T>& aRay) = default;
		Ray(const Vec3<T>& aOrigin, const Vec3<T>& aPoint)
			: origin(aOrigin), direction(aPoint - aOrigin) {}

		void InitWith2Points(const Vec3<T>& aOrigin, const Vec3<T>& aPoint)
			{ origin = aOrigin; direction = aPoint - aOrigin; }
		void InitWithOriginAndDirection(const Vec3<T>& aOrigin, const Vec3<T>& aDirection)
			{ origin = aOrigin; direction = aDirection; }
	};

	typedef Ray<float> Rayf;
}