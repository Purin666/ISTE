#pragma once

#include "ISTE/Math/Vec3.h"
#include <vector>

namespace ISTE
{
	class DebugDrawer;

	enum class NodeData
	{
		Open,
		Closed,
		Unvisited
	};
	struct TileData
	{
		int Length = INT_MAX;
		std::vector<CU::Vec3f> Points;
		std::vector<int> NeighborsIndex;
		NodeData NodeIs = NodeData::Unvisited;
		int BeforeTile = -1;
		int Index = -1;
		CU::Vec3f Center;
	};

	class Astar
	{
	public:

		Astar(std::vector<CU::Vec3f> somePositions, std::vector<CU::Vec3i> someTrianglePoints);
		~Astar() {}

		std::vector<int> FindPath(int aStartIndex, int aEndIndex);
		void Funneling(const CU::Vec3f& aStart, const CU::Vec3f& anEnd, std::vector<CU::Vec3f>& aPathOut);

		void ClearCache();
		inline std::vector<int>& GetPath() { return Path; }
		inline std::vector<TileData>& GetTriangleData() { return TrianglesData; }

		//int ManhattanDistance(TileData aNode, TileData aGoalNode);
		//std::vector<int> FindPath(int aStartIndex, int anEndIndex, int aLengthToCheck);
		//std::vector<int> Start(int aStartIndex, int anEndIndex);

	private:

		int TileCount; // How many triangles

		std::vector<TileData> TrianglesData;
		std::vector<int> Path;

		#ifdef _DEBUG
		DebugDrawer* LineDrawer = nullptr;
		#endif // _DEBUG


	};
}