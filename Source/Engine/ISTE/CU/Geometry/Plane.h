#pragma once
#include "ISTE/Math/Vec3.h"

namespace CU
{
	template <class T>
	class Plane
	{
	public:
		Plane() {}

		Plane(const Vec3<T>& aPoint0, const Vec3<T>& aPoint1, const Vec3<T>& aPoint2)
		{
			myPoint = aPoint0;
			myNormal = Vec3<T>(aPoint1 - aPoint0).Cross(aPoint2 - aPoint0);
		}

		Plane(const Vec3<T>& aPoint0, const Vec3<T>& aNormal)
		{
			myPoint = aPoint0;
			myNormal = aNormal;
		}

		void InitWith3Points(const Vec3<T>& aPoint0, const Vec3<T>& aPoint1, const Vec3<T>& aPoint2)
		{
			myPoint = aPoint0;
			myNormal = Vec3<T>(aPoint1 - aPoint0).Cross(aPoint2 - aPoint0);
		}

		void InitWithPointAndNormal(const Vec3<T>& aPoint, const Vec3<T>& aNormal)
		{
			myPoint = aPoint;
			myNormal = aNormal;
		}

		bool IsInside(const Vec3<T>& aPosition) const
		{
			return Vec3<T>(aPosition - myPoint).Dot(myNormal) <= 0;
		}

		const Vec3<T>& GetNormal() const
		{
			return myNormal;
		}

	private:
		Vec3<T> myPoint;
		Vec3<T> myNormal;
	};
	typedef Plane<float> Planef;
}