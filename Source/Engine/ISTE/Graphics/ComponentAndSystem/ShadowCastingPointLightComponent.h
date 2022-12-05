#pragma once 
#include <set>
#include "ISTE/Math/Vec.h"
#include "ISTE/Math/Matrix4x4.h"

#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/Graphics/RenderDefines.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelCommand.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimationCommand.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelCommand.h"


namespace ISTE {

	struct ShadowCastingPointLightComponent 
	{
		~ShadowCastingPointLightComponent() = default;

		CU::Vec4f myColorAndIntensity = { 1,1,1,1 };
		float myRadius = 1;
		CU::Matrix4x4f ProjectionMatrix;

		std::set<EntityID> myEntities;
	};
}