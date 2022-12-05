#pragma once

#include <string>
#include <vector>
#include "ISTE/Math/Vec3.h"

namespace ISTE
{

	class NavMeshImporter
	{
	public:

		NavMeshImporter(std::string);
		~NavMeshImporter();

		bool IsSafe() const { return myIsSafe; }

		std::vector<CU::Vec3f> GetPositions() const { return myPositions; }
		std::vector<CU::Vec3i> GetTriangles() const { return myTriangles; }

	private:

		bool myIsSafe;

		std::vector<CU::Vec3f> myPositions;
		std::vector<CU::Vec3i> myTriangles;

	};

}