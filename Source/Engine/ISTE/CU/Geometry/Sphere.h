#pragma once
#include "ISTE/Math/Vec3.h"

namespace CU
{
	template <class T>
	class Sphere
	{
	public:
		T radius;
		Vec3<T> center;

		Sphere() = default;
		Sphere(const Sphere<T>& aSphere) = default;
		Sphere(const Vec3<T>& aCenter, T aRadius) : center(aCenter), radius(aRadius) {}

		void InitWithCenterAndRadius(const Vec3<T>& aCenter, T aRadius) { center = aCenter; radius = aRadius; }

		bool IsInside(const Vec3<T>& aPosition) const
		{
			return (Vec3<T>(aPosition - center).LengthSqr() <= radius * radius);
		}
	};
}