#pragma once
#include "ISTE/Math/Vec.h"
#include "Plane.h"
#include "Ray.h"
#include "AABB2D.h"
#include "AABB3D.h"
#include "Circle.h"
#include "Sphere.h"

#include "ISTE/CU/UtilityFunctions.hpp"

#include <cmath>

namespace CU
{
	struct LineSegment
	{
		LineSegment() = default;
		LineSegment(const CU::Vec2f& aFrom, const CU::Vec2f& aTo)
			: myFrom(aFrom), myTo(aTo) {}
		LineSegment(const CU::Vec3f& aFrom, const CU::Vec3f& aTo)
			: myFrom(CU::Vec2f(aFrom.x, aFrom.z)), myTo(CU::Vec2f(aTo.x, aTo.z)) {}

		inline const CU::Vec2f GetNormal() { CU::Vec2f dir(myTo - myFrom); return CU::Vec2f(dir.y, -dir.x).GetNormalized(); }
		inline const CU::Vec2f GetMiddle() { return CU::Vec2f(myFrom + myTo) / 2.f; }
		CU::Vec2f myFrom;
		CU::Vec2f myTo;
	};

	struct Line3D
	{
		~Line3D() = default;
		CU::Vec3f myFrom;
		CU::Vec3f myTo;
	};

	struct Box3D
	{
		~Box3D() = default;
		CU::Vec3f myMin;
		CU::Vec3f myMax;
	};

// Declarations //////////////////////////////////////////////////

	// If the ray does not intersect the plane aOutIntersectionPoint remains unchanged and false is returned.
	// If the ray intersects the plane the intersection point is stored in aOutIntersectionPoint and true is returned.
	template <typename T>
	const bool IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, Vec3<T>& aOutIntersectionPoint);
	
	template <typename T>
	static const bool IntersectionSphereRay(const Sphere<T>& sphere, const Ray<T>& ray);
	template <typename T>
	static const bool IntersectionSphereRay(const Sphere<T>& sphere, const Ray<T>& ray, Vec3<T>& out);

	// Returns true if line segment 'p1q1' and 'p2q2' intersect. 
	template <typename T>
	const bool LineSegmentIntersect(const CU::Vec2<T>& p1, const CU::Vec2<T>& q1, const CU::Vec2<T>& p2, const CU::Vec2<T>& q2);
	
	// Returns true if the lines intersect, otherwise false. In addition, if the lines 
	// intersect the intersection point is stored in Vec2f& out.
	static const bool LineSegmentIntersect(const LineSegment& line0, const LineSegment& line1, CU::Vec2f& out);
	
	// Return true if aPos is inside 2D AABB
	template <typename T>
	const bool IsInside(const CU::Vec2<T>& aPos, const CU::Vec2<T>& aMin, const CU::Vec2<T>& aMax);

	// Return true if aPos is inside 3D AABB
	template <typename T>
	const bool IsInside(const CU::Vec3<T>& aPos, const CU::Vec3<T>& aMin, const CU::Vec3<T>& aMax);

	// Return true if aPos is inside triangle
	template<typename T>
	const bool IsInsideTriangle(const CU::Vec2<T>& aPos, const CU::Vec2<T>& p1, const CU::Vec2<T>& p2, const CU::Vec2<T>& p3);
	
	template<typename T>
	const bool IsInside(const CU::AABB2D<T> a, const CU::AABB2D<T> b);
	template<typename T>
	const bool IsInside(const CU::AABB3D<T> a, const CU::AABB3D<T> b);
	template<typename T>
	const bool IsInside(const CU::Circle<T> a, const CU::Circle<T> b);
	template<typename T>
	const bool IsInside(const CU::Sphere<T> a, const CU::Sphere<T> b);
	template<typename T>
	const bool IsInside(const CU::AABB2D<T> box, const CU::Circle<T> cir);
	template<typename T>
	const bool IsInside(const CU::AABB3D<T> box, const CU::Sphere<T> sph);


	//non-templated
	static bool GetIntersection(float fDst1, float fDst2, const CU::Vec3f& P1, const CU::Vec3f& P2, CU::Vec3f& Hit) {
		if ((fDst1 * fDst2) >= 0.0f) return 0;
		if (fDst1 == fDst2) return 0;
		Hit = P1 + (P2 - P1) * (-fDst1 / (fDst2 - fDst1));
		return 1;
	}

	static bool InBox(CU::Vec3f Hit, CU::Vec3f B1, CU::Vec3f B2, const int Axis) {
		if (Axis == 1 && Hit.z > B1.z && Hit.z < B2.z && Hit.y > B1.y && Hit.y < B2.y) return 1;
		if (Axis == 2 && Hit.z > B1.z && Hit.z < B2.z && Hit.x > B1.x && Hit.x < B2.x) return 1;
		if (Axis == 3 && Hit.x > B1.x && Hit.x < B2.x && Hit.y > B1.y && Hit.y < B2.y) return 1;
		return 0;
	}

	static bool LineBoxIntersection(const Line3D& aLine, const Box3D& aBox)
	{
		if (aLine.myTo.x < aBox.myMin.x && aLine.myFrom.x < aBox.myMin.x) return false;
		if (aLine.myTo.x > aBox.myMax.x && aLine.myFrom.x > aBox.myMax.x) return false;
		if (aLine.myTo.y < aBox.myMin.y && aLine.myFrom.y < aBox.myMin.y) return false;
		if (aLine.myTo.y > aBox.myMax.y && aLine.myFrom.y > aBox.myMax.y) return false;
		if (aLine.myTo.z < aBox.myMin.z && aLine.myFrom.z < aBox.myMin.z) return false;
		if (aLine.myTo.z > aBox.myMax.z && aLine.myFrom.z > aBox.myMax.z) return false;

		if (aLine.myFrom.x > aBox.myMin.x && aLine.myFrom.x < aBox.myMax.x &&
			aLine.myFrom.y > aBox.myMin.y && aLine.myFrom.y < aBox.myMax.y &&
			aLine.myFrom.z > aBox.myMin.z && aLine.myFrom.z < aBox.myMax.z)
		{
			//Hit = L1;
			return true;
		}

		CU::Vec3f hit;

		if ((GetIntersection(aLine.myFrom.x - aLine.myTo.x, aLine.myTo.x - aLine.myTo.x, aLine.myFrom, aLine.myTo, hit) && InBox(hit, aBox.myMin, aBox.myMax, 1))
			|| (GetIntersection(aLine.myFrom.y - aLine.myTo.y, aLine.myTo.y - aLine.myTo.y, aLine.myFrom, aLine.myTo, hit) && InBox(hit, aBox.myMin, aBox.myMax, 2))
			|| (GetIntersection(aLine.myFrom.z - aLine.myTo.z, aLine.myTo.z - aLine.myTo.z, aLine.myFrom, aLine.myTo, hit) && InBox(hit, aBox.myMin, aBox.myMax, 3))
			|| (GetIntersection(aLine.myFrom.x - aBox.myMax.x, aLine.myTo.x - aBox.myMax.x, aLine.myFrom, aLine.myTo, hit) && InBox(hit, aBox.myMin, aBox.myMax, 1))
			|| (GetIntersection(aLine.myFrom.y - aBox.myMax.y, aLine.myTo.y - aBox.myMax.y, aLine.myFrom, aLine.myTo, hit) && InBox(hit, aBox.myMin, aBox.myMax, 2))
			|| (GetIntersection(aLine.myFrom.z - aBox.myMax.z, aLine.myTo.z - aBox.myMax.z, aLine.myFrom, aLine.myTo, hit) && InBox(hit, aBox.myMin, aBox.myMax, 3)))
			return true;

		return false;
	}

}



// Implementations //////////////////////////////////////////////////

template <typename T>
const bool CU::IntersectionPlaneRay(const Plane<T>& aPlane, const Ray<T>& aRay, Vec3<T>& aOutIntersectionPoint)
{
	// DISCLAIMER:: NOT SURE IF THIS IS WORKING AS INTENDED
	T t = (aRay.direction.GetNormalized().Length() - (aRay.origin.Dot(aPlane.GetNormal()))) / (aRay.direction.Dot(aPlane.GetNormal()));

	if (aPlane.IsInside(aRay.origin + aRay.direction.GetNormalized() * t))
	{
		aOutIntersectionPoint = aRay.origin + aRay.direction.GetNormalized() * t;
		return true;
	}
	return false;
}

template <typename T>
const bool CU::IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay)
{
	const Vec3<T> e = aSphere.center - aRay.origin;
	const T a = e.Dot(aRay.direction.GetNormalized());
	const T t = a - std::sqrt(aSphere.radius * aSphere.radius - e.LengthSqr() + a * a);

	if (aSphere.IsInside(aRay.origin + aRay.direction.GetNormalized() * a))
		return t >= 0;

	return false;
}

template <typename T>
const bool CU::IntersectionSphereRay(const Sphere<T>& aSphere, const Ray<T>& aRay, Vec3<T>& aOutIntersectionPoint)
{
	const Vec3<T> e = aSphere.center - aRay.origin;
	const T a = e.Dot(aRay.direction.GetNormalized());

	if (!aSphere.IsInside(aRay.origin + aRay.direction.GetNormalized() * a))
		return false;

	const T t = a - std::sqrt(aSphere.radius * aSphere.radius - e.LengthSqr() + a * a);

	if (t < 0) return false;

	aOutIntersectionPoint = aRay.origin + aRay.direction.GetNormalized() * t;
	return true;
}

namespace // Helpers LineSegmentIntersect
{
	// Given three collinear points p, q, r, the function checks if 
	// point q lies on line segment 'pr' 
	template <typename T>
	const bool OnSegment(const CU::Vec2<T>& p, const CU::Vec2<T>& q, const CU::Vec2<T>& r)
	{
		if (q.x <= max(p.x, r.x) && q.x >= min(p.x, r.x) &&
			q.y <= max(p.y, r.y) && q.y >= min(p.y, r.y))
			return true;

		return false;
	}
	template <typename T>
	const int LineOrientation(const CU::Vec2<T>& p, const CU::Vec2<T>& q, const CU::Vec2<T>& r)
	{
		// See https://www.geeksforgeeks.org/orientation-3-ordered-points/ for details of below formula. 
		const int val = (q.y - p.y) * (r.x - q.x) - (q.x - p.x) * (r.y - q.y);

		if (val == 0) return 0;  // collinear 

		return (val > 0) ? 1 : 2; // clock or counterclock wise 
	}
}

template <typename T>
const bool CU::LineSegmentIntersect(const CU::Vec2<T>& p1, const CU::Vec2<T>& q1, const CU::Vec2<T>& p2, const CU::Vec2<T>& q2)
{
	const int o1 = LineOrientation(p1, q1, p2);
	const int o2 = LineOrientation(p1, q1, q2);
	const int o3 = LineOrientation(p2, q2, p1);
	const int o4 = LineOrientation(p2, q2, q1);

	if (o1 != o2 && o3 != o4) return true;

	if (o1 == 0 && OnSegment(p1, p2, q1)) return true;	// p1, q1 and p2 are collinear and p2 lies on segment p1q1 
	if (o2 == 0 && OnSegment(p1, q2, q1)) return true;	// p1, q1 and q2 are collinear and q2 lies on segment p1q1 
	if (o3 == 0 && OnSegment(p2, p1, q2)) return true;	// p2, q2 and p1 are collinear and p1 lies on segment p2q2 
	if (o4 == 0 && OnSegment(p2, q1, q2)) return true;	// p2, q2 and q1 are collinear and q1 lies on segment p2q2 

	return false; // Doesn't fall in any of the above cases 
}

const bool CU::LineSegmentIntersect(const LineSegment& line0, const LineSegment& line1, CU::Vec2f& out)
{
	const CU::Vec2f s1(line0.myTo.x - line0.myFrom.x, line0.myTo.y - line0.myFrom.y);
	const CU::Vec2f s2(line1.myTo.x - line1.myFrom.x, line1.myTo.y - line1.myFrom.y);

	const float s = (-s1.y * (line0.myFrom.x - line1.myFrom.x) + s1.x * (line0.myFrom.y - line1.myFrom.y)) / (-s2.x * s1.y + s1.x * s2.y);
	const float t = ( s2.x * (line0.myFrom.y - line1.myFrom.y) - s2.y * (line0.myFrom.x - line1.myFrom.x)) / (-s2.x * s1.y + s1.x * s2.y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{
		// Collision detected
		out.x = line0.myFrom.x + (t * s1.x);
		out.y = line0.myFrom.y + (t * s1.y);
		return true;
	}
	return false; // No collision
}

//// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines 
//// intersect the intersection point may be stored in the floats i_x and i_y.
//char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float* i_x, float* i_y)
//{
//	float s1_x, s1_y, s2_x, s2_y;
//	s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
//	s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;
//
//	float s, t;
//	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
//	t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);
//
//	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
//	{
//		// Collision detected
//		if (i_x != NULL)
//			*i_x = p0_x + (t * s1_x);
//		if (i_y != NULL)
//			*i_y = p0_y + (t * s1_y);
//		return 1;
//	}
//
//	return 0; // No collision
//}


template <typename T>
const bool CU::IsInside(const CU::Vec2<T>& aPos, const CU::Vec2<T>& aMin, const CU::Vec2<T>& aMax)
{
	return	aMin.x < aPos.x &&
			aMax.x > aPos.x &&
			aMin.y < aPos.y &&
			aMax.y > aPos.y;
}

template<typename T>
const bool CU::IsInside(const CU::Vec3<T>& aPos, const CU::Vec3<T>& aMin, const CU::Vec3<T>& aMax)
{
	return	aMin.x < aPos.x &&
			aMax.x > aPos.x &&
			aMin.y < aPos.y &&
			aMax.y > aPos.y &&
			aMin.z < aPos.z &&
			aMax.z > aPos.z;
}

namespace // Helpers IsInsideTriangle
{
	template<typename T>
	const float Sign(const CU::Vec2<T>& p1, const CU::Vec2<T>& p2, const CU::Vec2<T>& p3)
	{
		return (p1.x - p3.x) * (p2.y - p3.y) - (p2.x - p3.x) * (p1.y - p3.y);
	}
}

template<typename T>
const bool CU::IsInsideTriangle(const CU::Vec2<T>& aPos, const CU::Vec2<T>& p1, const CU::Vec2<T>& p2, const CU::Vec2<T>& p3)
{
	const T f1 = Sign(aPos, p1, p2);
	const T f2 = Sign(aPos, p2, p3);
	const T f3 = Sign(aPos, p3, p1);

	const bool hasNeg = (f1 < 0) || (f2 < 0) || (f3 < 0);
	const bool hasPos = (f1 > 0) || (f2 > 0) || (f3 > 0);

	return !(hasNeg && hasPos);
}

template<typename T>
const bool CU::IsInside(const CU::AABB2D<T> a, const CU::AABB2D<T> b)
{
	return	a.min.x <= b.max.x &&
			a.min.y <= b.max.y &&
			b.min.x <= a.max.x &&
			b.min.y <= a.max.y;
}

template<typename T>
const bool CU::IsInside(const CU::AABB3D<T> a, const CU::AABB3D<T> b)
{
	return	a.min.x <= b.max.x &&
			a.min.y <= b.max.y &&
			a.min.z <= b.max.z &&
			b.min.x <= a.max.x &&
			b.min.y <= a.max.y &&
			b.min.z <= a.max.z;
}

template<typename T>
const bool CU::IsInside(const CU::Circle<T> a, const CU::Circle<T> b)
{
	const T sumRadii = a.radius + b.radius;
	const T distSqr = Vec2<T>(a.center - b.center).LengthSqr();
	const T rangeSqr = sumRadii * sumRadii;

	return distSqr <= rangeSqr;
}

template<typename T>
const bool CU::IsInside(const CU::Sphere<T> a, const CU::Sphere<T> b)
{
	const T sumRadii = a.radius + b.radius;
	const T distSqr = Vec3<T>(a.center - b.center).LengthSqr();
	const T rangeSqr = sumRadii * sumRadii;

	return distSqr <= rangeSqr;
}

template<typename T>
const bool CU::IsInside(const CU::AABB2D<T> box, const CU::Circle<T> cir)
{
	const T hx = (box.max.x - box.min.x) * 0.5f;
	const T hy = (box.max.y - box.min.y) * 0.5f;

	const CU::Vec2<T> boxPos(box.min.x + hx,
							 box.min.y + hy);

	const CU::Vec2<T> dist(CU::Abs(cir.center.x - boxPos.x),
						   CU::Abs(cir.center.y - boxPos.y));

	if (dist.x > (hx + cir.radius)) return false;
	if (dist.y > (hy + cir.radius)) return false;

	if (dist.x <= hx) return true;
	if (dist.y <= hy) return true;

	const T cornerDistSqr = (dist.x - hx) * (dist.x - hx) +
							(dist.y - hy) * (dist.y - hy);

	return (cornerDistSqr <= (cir.radius * cir.radius));
}

template<typename T>
const bool CU::IsInside(const CU::AABB3D<T> box, const CU::Sphere<T> sph)
{
	const T hx = (box.max.x - box.min.x) * 0.5f;
	const T hy = (box.max.y - box.min.y) * 0.5f;
	const T hz = (box.max.z - box.min.z) * 0.5f;

	const CU::Vec3<T> boxPos(box.min.x + hx,
							 box.min.y + hy,
							 box.min.z + hz);

	const CU::Vec3<T> dist(CU::Abs(sph.center.x - boxPos.x),
						   CU::Abs(sph.center.y - boxPos.y),
						   CU::Abs(sph.center.z - boxPos.z));

	if (dist.x > (hx + sph.radius)) return false;
	if (dist.y > (hy + sph.radius)) return false;
	if (dist.z > (hz + sph.radius)) return false;

	if (dist.x <= hx) return true;
	if (dist.y <= hy) return true;
	if (dist.z <= hz) return true;

	const T cornerDistSqr = (dist.x - hx) * (dist.x - hx) +
		(dist.y - hy) * (dist.y - hy) +
		(dist.z - hz) * (dist.z - hz);

	return (cornerDistSqr <= (sph.radius * sph.radius));
}

