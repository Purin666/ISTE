#pragma once

#include "ISTE/ECSB/System.h"

#include "ISTE/CU/Geometry/Intersection.h"

namespace ISTE
{
	class TriggerSystem : public System
	{
	public:
		enum class MapValues
		{
			Box,
			Sphere
		};

	public:
		void Check();

	private:
		friend class VisualizationSettings;


		void BoxBox();
		void BoxSphere();
		void SphereSphere();

		void DrawBoxes();
		void DrawSpheres();

		void DrawBox(const CU::AABB3D<float>& aBox);
		void DrawSphere(const CU::Sphere<float>& aSphere);
		void Draw3DSphere(const CU::Sphere<float>& aSphere, const size_t aRes = 32);
	};
}