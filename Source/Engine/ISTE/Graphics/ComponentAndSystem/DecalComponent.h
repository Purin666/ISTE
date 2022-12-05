#pragma once
#include "ISTE/Math/Vec3.h"
#include "ISTE/Graphics/RenderDefines.h"

namespace ISTE{
	struct DecalComponent{ 
		CU::Vec3f myLerpValues;

		float myAngleThreshold;

		//what the decal is allowed to draw on
		RenderFlags myCoverageFlag = RenderFlags::Environment;
	};
}