#pragma once
#include "ISTE/Math/Vec3.h"

namespace CU
{
	template <class T>
	class AABB3D
	{
	public:
		Vec3<T> min, max;

		AABB3D() = default;
		AABB3D(const AABB3D<T>& aAABB3D) = default;
		AABB3D(const Vec3<T>& aMin, const Vec3<T>& aMax) : min(aMin), max(aMax) {}

		void InitWithMinAndMax(const Vec3<T>& aMin, const Vec3<T>& aMax) { min = aMin; max = aMax; }

		bool IsInside(const Vec3<T>& aPosition) const
		{
			return	(aPosition.x >= min.x && aPosition.x <= max.x) &&
					(aPosition.y >= min.y && aPosition.y <= max.y) &&
					(aPosition.z >= min.z && aPosition.z <= max.z);
		}
	};
}