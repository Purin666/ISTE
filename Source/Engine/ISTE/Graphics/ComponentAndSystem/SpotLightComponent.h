#pragma once
#include "ISTE/Math/Vec.h"
#include "ISTE/Math/Matrix4x4.h"
namespace ISTE {
	struct SpotLightComponent {
		CU::Vec4f myColorAndIntensity	= {1,1,1,1};
		float myRange					= 1.f;
		float myOuterAngle				= 1.2f;
		float myInnerAngle				= 0.5f;
		bool myShadowCastingFlag		= false; 
	};
}