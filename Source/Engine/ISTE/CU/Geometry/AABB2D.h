#pragma once
#include "ISTE/Math/Vec2.h"

namespace CU
{
    template <class T>
	class AABB2D
	{
	public:
		Vec2<T> min, max;

		AABB2D() = default;
		AABB2D(const AABB2D<T>& aAABB2D) = default;
		AABB2D(const Vec2<T>& aMin, const Vec2<T>& aMax) : min(aMin), max(aMax) {}

		void InitWithMinAndMax(const Vec2<T>& aMin, const Vec2<T>& aMax) { min = aMin; max = aMax; }
		
		bool IsInside(const Vec2<T>& aPosition) const
		{
			return	(aPosition.x >= min.x && aPosition.x <= max.x) &&
					(aPosition.y >= min.y && aPosition.y <= max.y);
		}
	};
}