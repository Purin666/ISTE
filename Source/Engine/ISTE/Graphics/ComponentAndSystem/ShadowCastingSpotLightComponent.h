#pragma once 
#include <unordered_map>
#include <vector>
#include <set>

#include "ISTE/Math/Matrix4x4.h"
#include "ISTE/Math/Vec.h"
#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/Graphics/ComponentAndSystem/ModelCommand.h"

namespace ISTE {
	struct ShadowCastingSpotLightComponent 
	{
		~ShadowCastingSpotLightComponent() = default;

		CU::Vec4f myColorAndIntensity = { 1,1,1,1 };
		float myRange = 1.f;
		float myOuterAngle = 1.2f;
		float myInnerAngle = 0.5f;
		CU::Matrix4x4f ProjectionMatrix;

		std::set<EntityID> myEntities;
	};
}