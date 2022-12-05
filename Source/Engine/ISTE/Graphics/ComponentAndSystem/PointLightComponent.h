#pragma once
#include "ISTE/Math/Vec.h"
#include "ISTE/Math/Matrix4x4.h"

namespace ISTE {
	struct PointLightComponent {
		CU::Vec4f myColorAndIntensity	= {1,1,1,1};
		float myRadius					= 1;
		bool myShadowCastingFlag		= false; 
	};
}