#pragma once
#include "ISTE/Math/Vec.h"

namespace ISTE {
	struct BillboardComponent {
		CU::Vec3f myColor		= CU::Vec3f(1, 1, 1);
		CU::Vec2f myUVStart		= CU::Vec2f(0, 0);		//x:left	y:top
		CU::Vec2f myUVEnd		= CU::Vec2f(1, 1);		//x:right	y:bottom
		CU::Vec2f myUVOffset	= CU::Vec2f(0, 0);
		CU::Vec2f myUVScale		= CU::Vec2f(1, 1);

		SamplerState mySamplerState = SamplerState::eTriLinear;
		AdressMode myAdressMode		= AdressMode::eClamp;
	};
};