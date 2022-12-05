#pragma once

#include "ISTE/Math/Vec.h"
#include <array>
#include <vector>

namespace ISTE
{
namespace NavMesh
{
	enum class NodeStatus
	{
		Unvisited,
		Open,
		Closed,
	};
	struct NavMeshTriangle
	{
		std::array<CU::Vec3f*, 3> vertices;			// TODO:: There's no need to store copies of vertices when we have the indices for them. Though we have Center() for now that use them. // Mathias
		std::array<unsigned int, 3> vertexIndices;	// TODO:: std::array for iterator, not used yet though // Mathias
		int index = -1;
		std::vector<int> connections;
		
		__forceinline const CU::Vec3f Center() const { return CU::Vec3f((*vertices[0] + *vertices[1] + *vertices[2]) / 3.f); }
	};
	struct NavMeshNode
	{
		NavMeshNode* parent;
		int index;
		int G = 0;
		float H = 0;
		float F = FLT_MAX;
		NodeStatus status = NodeStatus::Unvisited;

		NavMeshNode() = default;
		NavMeshNode(NavMeshNode* aParent, int anIndex, NodeStatus aStatus)
			: parent(aParent), index(anIndex), status(aStatus) {}
	};
	struct NavMesh
	{
		std::vector<NavMeshTriangle> triangles;
		std::vector<CU::Vec3f> vertices;
	};
}
}