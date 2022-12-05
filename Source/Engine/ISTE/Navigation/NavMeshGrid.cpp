#include "NavMeshGrid.h"

#include "ISTE/Context.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/DebugDrawer.h"

#include "ISTE/CU/Geometry/Intersection.h"

// just for getting player pos
#include "ISTE/Scene/Scene.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
// end

#include <bitset>

void ISTE::NavMesh::Grid::Init(CU::Vec2f& aMin, size_t aWidth, size_t aHeight, size_t aCellSize)
{
	myWidth = aWidth;
	myHeight = aHeight;
	myCellSize = aCellSize;

	myGridCells.resize(myWidth * myHeight);

	for (size_t row = 0; row < myHeight; row++)
	{
		for (size_t col = 0; col < myWidth; col++)
		{
			const size_t index = col + row * myHeight;
			const float x = static_cast<float>(col * myCellSize);
			const float y = static_cast<float>(row * myCellSize);
			myGridCells[index].myMin = CU::Vec2f( x, y ) + aMin;
			myGridCells[index].myMax = CU::Vec2f( x + (float)myCellSize, y + (float)myCellSize ) + aMin;
		}
	}

	//DebugDrawer& drawer = Context::Get()->myGraphicsEngine->GetDebugDrawer();
	//LineCommand line;
	//for (auto& cell : myGridCells)
	//{
	//	const CU::Vec3f from = CU::Vec3f(cell.myMin.x, 0, cell.myMin.y);
	//	const CU::Vec3f toX = CU::Vec3f(cell.myMax.x, 0, cell.myMin.y);
	//	const CU::Vec3f toY = CU::Vec3f(cell.myMin.x, 0, cell.myMax.y);

	//	line.myColor = CU::Vec3f(0, 1, 1);
	//	line.myFromPosition = from;
	//	line.myToPosition = toX;
	//	drawer.AddStaticLineCommand(line);
	//	line.myToPosition = toY;
	//	drawer.AddStaticLineCommand(line);
	//}

}

void ISTE::NavMesh::Grid::AddTriangle(NavMeshTriangle* aTriangle)
{
	DebugDrawer& drawer = Context::Get()->myGraphicsEngine->GetDebugDrawer();
	LineCommand line;

	// new
	for (auto& cell : myGridCells)
	{
		// points of the triangle
		const CU::Vec2f t1 = CU::Vec2f(aTriangle->vertices[0]->x, aTriangle->vertices[0]->z);
		const CU::Vec2f t2 = CU::Vec2f(aTriangle->vertices[1]->x, aTriangle->vertices[1]->z);
		const CU::Vec2f t3 = CU::Vec2f(aTriangle->vertices[2]->x, aTriangle->vertices[2]->z);
		// points of the cell
		const CU::Vec2f c1 = CU::Vec2f(cell.myMin.x, cell.myMin.y);
		const CU::Vec2f c2 = CU::Vec2f(cell.myMax.x, cell.myMin.y);
		const CU::Vec2f c3 = CU::Vec2f(cell.myMax.x, cell.myMax.y);
		const CU::Vec2f c4 = CU::Vec2f(cell.myMin.x, cell.myMax.y);

		// if any triangle vertice is inside cell
		bool any =		CU::IsInside(t1, cell.myMin, cell.myMax) ||
						CU::IsInside(t2, cell.myMin, cell.myMax) ||
						CU::IsInside(t3, cell.myMin, cell.myMax);
		// if any cell vertice is inside triangle
		if (!any) any = CU::IsInsideTriangle(c1, t1, t2, t3) ||
						CU::IsInsideTriangle(c2, t1, t2, t3) ||
						CU::IsInsideTriangle(c3, t1, t2, t3) ||
						CU::IsInsideTriangle(c4, t1, t2, t3);
		// if any lines intersects
		if (!any) any = CU::LineSegmentIntersect(c1, c2, t1, t2) ||
						CU::LineSegmentIntersect(c1, c2, t2, t3) ||
						CU::LineSegmentIntersect(c1, c2, t3, t1);
		if (!any) any = CU::LineSegmentIntersect(c2, c3, t1, t2) ||
						CU::LineSegmentIntersect(c2, c3, t2, t3) ||
						CU::LineSegmentIntersect(c2, c3, t3, t1);
		if (!any) any = CU::LineSegmentIntersect(c3, c4, t1, t2) ||
						CU::LineSegmentIntersect(c3, c4, t2, t3) ||
						CU::LineSegmentIntersect(c3, c4, t3, t1);
		if (!any) any = CU::LineSegmentIntersect(c4, c1, t1, t2) ||
						CU::LineSegmentIntersect(c4, c1, t2, t3) ||
						CU::LineSegmentIntersect(c4, c1, t3, t1);

		if (any)
		{
			cell.myTriangles.push_back(aTriangle);
//#ifdef _DEBUG
//			const float halfSize = (float)myCellSize * 0.5f;
//			const CU::Vec3f from = CU::Vec3f(cell.myMin.x + halfSize, 0, cell.myMin.y + halfSize);
//			const CU::Vec3f to = CU::Vec3f(aTriangle->Center());
//
//			line.myColor = CU::Vec3f(1, 0, 0);
//			line.myFromPosition = from;
//			line.myToPosition = to;
//			drawer.AddStaticLineCommand(line);
//#endif // _DEBUG
		}
	}

	//for (auto& vert : aTriangle->vertices)
	//{
	//	for (auto& cell : myGridCells)
	//	{
	//		if (IsInside(vert, cell))
	//		{
	//			cell.myTriangles.push_back(aTriangle);
	//
	//			const float halfSize = (float)myCellSize * 0.5f;
	//			const CU::Vec3f from = CU::Vec3f(cell.myMin.x + halfSize, 0, cell.myMin.y + halfSize);
	//			const CU::Vec3f to = CU::Vec3f(aTriangle->Center());
	//			
	//			line.myColor = CU::Vec3f(1, 0, 0);
	//			line.myFromPosition = from;
	//			line.myToPosition = to;
	//			drawer.AddStaticLineCommand(line);
	//		}
	//	}
	//}
}

const bool ISTE::NavMesh::Grid::GetTriangles(const CU::Vec3f& aRayOri, const CU::Vec3f& aRayDir, std::vector<ISTE::NavMesh::NavMeshTriangle*>& someTrianglesOut)
{
	// Intersect plane, find intersection point
	//EntityID id = Context::Get()->mySceneHandler->GetActiveScene().GetPlayerId();
	//const CU::Vec3f planePoint = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(id)->myPosition;
	//const CU::Vec3f planeNormal = CU::Vec3f(0, 1, 0);
	//
	//const CU::Planef plane(planePoint, planeNormal);
	//CU::Rayf ray;
	//ray.InitWithOriginAndDirection(aRayOri, aRayDir);
	//
	//CU::Vec3f intersectPoint;
	//if (CU::IntersectionPlaneRay(plane, ray, intersectPoint))
	//	return false;

	const CU::Vec3f planeNormal = CU::Vec3f(0, 1, 0);
	float t = (aRayDir.GetNormalized().Length() - aRayOri.Dot(planeNormal)) / aRayDir.Dot(planeNormal);
	const CU::Vec3f intersectPoint = aRayOri + aRayDir.GetNormalized() * t;

	// Find cell hit by intersection point
	for (auto& cell : myGridCells)
	{
		if (CU::IsInside(CU::Vec2f(intersectPoint.x, intersectPoint.z), cell.myMin, cell.myMax))
		{
			someTrianglesOut = cell.myTriangles;
			return true; // Return triangles of the cell hit
		}
	}
	return false;
}

