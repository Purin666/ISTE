#pragma once
#include "ISTE/Math/Vec3.h"

namespace CU
{
	class Plane
	{
	private:
		
		Vec3f myPoint;
		Vec3f myNormal;

	public:
		// Default constructor.
		Plane()
		{
		}

		// Constructor taking three points where the normal is (aPoint1 - aPoint0) x (aPoint2-aPoint0).
		Plane(const Vec3f& aPoint0, const Vec3f& aPoint1, const Vec3f& aPoint2)
		{
			myPoint = aPoint0;
			myNormal = Vec3f(aPoint1 - aPoint0).Cross(aPoint2 - aPoint0);
		}

		// Constructor taking a point and a normal.
		Plane(const Vec3f& aPoint, const Vec3f& aNormal)
		{
			myPoint = aPoint;
			myNormal = aNormal;
		}

		// Init the plane with three points, the same as the constructor above.
		void InitWith3Points(const Vec3f& aPoint0, const Vec3f& aPoint1, const Vec3f& aPoint2)
		{
			myPoint = aPoint0;
			myNormal = Vec3f(aPoint1 - aPoint0).Cross(aPoint2 - aPoint0);
		}

		// Init the plane with a point and a normal, the same as the constructor above.
		void InitWithPointAndNormal(const Vec3f& aPoint, const Vec3f& aNormal)
		{
			myPoint = aPoint;
			myNormal = aNormal;
		}

		// Set Pos
		inline void SetPos(const Vec3f& aPoint) { myPoint = aPoint; };

		// Invert Normal
		inline void InvertNormal() { myNormal = myNormal * -1.f; };

		// Returns whether a point is inside the plane: it is inside when the point is on the plane or on the side the normal is pointing away from.
		bool IsInside(const Vec3f& aPosition) const
		{
			return Vec3f(aPosition - myPoint).Dot(myNormal) <= 0;
		}

		// Returns the normal of the plane.
		const Vec3f& GetNormal() const
		{
			return myNormal;
		}
	};
}