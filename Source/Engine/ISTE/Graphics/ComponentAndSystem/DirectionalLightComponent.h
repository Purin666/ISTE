#pragma once
#include "ISTE/Math/Vec.h"
#include "ISTE/Graphics/Camera.h"

namespace ISTE {
	struct DirectionalLightComponent {
		CU::Vec4f myColorAndIntensity;
		
		Camera myLightSpaceCam;
	};
}