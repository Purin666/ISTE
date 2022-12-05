#pragma once
#include "ISTE/Graphics/RenderDefines.h"
#include "ISTE/Graphics/RenderStateEnums.h" 
#include <ISTE/Math/Vec2.h> 
#include <ISTE/Math/Vec3.h> 

namespace ISTE
{
	struct Sprite2DComponent
	{
		CU::Vec3f myColor		= CU::Vec3f(1, 1, 1);
		CU::Vec2f myUVStart		= CU::Vec2f(0, 0);		//x:left	y:top
		CU::Vec2f myUVEnd		= CU::Vec2f(1, 1);		//x:right	y:bottom
		CU::Vec2f myUVOffset	= CU::Vec2f(0, 0);
		CU::Vec2f myUVScale		= CU::Vec2f(1, 1);

		TextureID myTextureId	= -1;

		SamplerState mySamplerState = SamplerState::eTriLinear;
		AdressMode myAdressMode = AdressMode::eClamp;
	};
}