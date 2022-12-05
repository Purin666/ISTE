#pragma once

#include "NavMeshDefines.h"


namespace ISTE
{
	namespace NavMesh
	{
		struct Plane
		{
			CU::Vec3f myPoint;
			CU::Vec3f myNormal;
		};
		struct GridCell
		{
			CU::Vec2f myMin;
			CU::Vec2f myMax;
			std::vector<NavMeshTriangle*> myTriangles;
		};
		class Grid
		{
		public:
			Grid() = default;

			void Init(CU::Vec2f& aMin, size_t aWidth, size_t aHeight, size_t aCellSize);

			void AddTriangle(NavMeshTriangle* aTriangle);

			const bool GetTriangles(const CU::Vec3f& aRayOri, const CU::Vec3f& aRayDir, std::vector<ISTE::NavMesh::NavMeshTriangle*>& someTrianglesOut);
			inline const std::vector<GridCell>& GetCells() { return myGridCells; };

		private:
			//const bool IsInside(const CU::Vec3f& aVert, GridCell& aCell);
			inline const size_t GetCol(const size_t anIndex) { return anIndex % myWidth; }
			inline const size_t GetRow(const size_t anIndex) { return anIndex / myHeight; }

			std::vector<GridCell> myGridCells;
			size_t myWidth;
			size_t myHeight;
			size_t myCellSize;
			CU::Vec2f myMin;
		};

	}
}