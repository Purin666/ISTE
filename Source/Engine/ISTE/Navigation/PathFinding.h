#pragma once
#include "NavMeshDefines.h"
#include <queue>



namespace ISTE
{
	namespace NavMesh
	{

		struct Intersection
		{
			std::vector<CU::Vec3f> myIntersections;
			int myTriangleIndex = -1;
		};

		const int FindTriangleIndexFromPos(const CU::Vec3f& aPos, ISTE::NavMesh::NavMesh& aNavMesh);
		const int FindTriangleIndexFromPosInside(const CU::Vec3f& aPos, ISTE::NavMesh::NavMesh& aNavMesh);
		const bool RayIntersectsNavMesh(const CU::Vec3f& aRayOri, const CU::Vec3f& aRayDir, CU::Vec3f& anOutIntersectionPoint);
		
		// Use this when RayIntersectsNavMesh() return false;
		const bool RayIntersectPlane(const CU::Vec3f& aRayOri, const CU::Vec3f& aRayDir, const CU::Vec3f& aStart, CU::Vec3f& anOutIntersectionPoint);
		const bool RayIntersectionOff(const CU::Vec3f& aRayOri, const CU::Vec3f& aRayDir, float aNonResponsiveAngle, const CU::Vec3f& aStart, CU::Vec3f& anOutIntersectionPoint);

		const bool RayClosestPointIntersection(const CU::Vec3f& aRayOri, const CU::Vec3f& aRayDir, float aNonResponsiveAngle, const CU::Vec3f& aStart, CU::Vec3f& anOutIntersectionPoint, float aMaxDist = FLT_MAX, bool aIgnoreProjection = false);
		void GetIntersections(NavMesh& aNavMesh, const CU::Vec3f& aStart, const CU::Vec3f& aDestination, std::vector<Intersection>& aOutIntersections);

		//This function dosn't calculate if the point is inside the triangle meaning if the point is outside
		//it will return what y would be if the triangle extended that far
		float GetAproximatedY(ISTE::NavMesh::NavMeshTriangle& aTri, const CU::Vec3f& aPoint);
		//If the destination point lies outside of the navmesh this can find the closest edge
		CU::Vec3f GetRealisticPointInNavmeshFromPoint(ISTE::NavMesh::NavMesh& aNavMesh, const CU::Vec3f& aStart, const CU::Vec3f& aDestination);
		// Takes a Start Position and a Destination Position
		// Returns the index-path
		// ! Using A-Star (I think) // Mathias
		std::vector<int> FindIndexPath(const CU::Vec3f& aStart, const CU::Vec3f& aDestination);

		// Takes a Start Position, a Destination Position and a std::vector of position to move to
		// Fills the std::vector with positions to move to
		// ! Funneling
		void FindPointPath(const CU::Vec3f& aStart, const CU::Vec3f& aDestination, std::queue<CU::Vec3f>& aPathOut, bool aIgnoreExtraPoints = false);

		// Takes a Start Position, a Destination Position and a NavMesh
		// Returns the index-path
		// ! Using Djikstra
		//std::vector<int> FindIndexPath(const CU::Vec3f& aStart, const CU::Vec3f& aDestination, NavMesh& aNavMesh);
	}

	namespace MovementMath
	{
		const CU::Vec2f GetUnitVector(const CU::Vec3f& aStart, const CU::Vec3f& aEnd);
		CU::Vec2f GetUnitVector(CU::Vec3f& aStart, CU::Vec3f& aEnd, float& aLength);

		const CU::Vec3f GetUnitVector3(CU::Vec3f& aStart, CU::Vec3f& aEnd);
		CU::Vec3f GetUnitVector3(CU::Vec3f& aStart, CU::Vec3f& aEnd, float& aLength);
	}
}

