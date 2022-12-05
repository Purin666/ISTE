#pragma once
#include "ISTE/Graphics/RenderStateEnums.h" 
#include <ISTE/Math/Vec2.h> 
#include <ISTE/Math/Vec3.h> 
#include "ISTE/Graphics/RenderDefines.h"

namespace ISTE
{
	struct Sprite3DComponent
	{
		CU::Vec3f myColor = { 1,1,1 };
		CU::Vec2f myUVStart;	//x:left	y:top
		CU::Vec2f myUVEnd;		//x:right	y:bottom
		CU::Vec2f myUVOffset;
		CU::Vec2f myUVScale;

		TextureID myTextureId = -1;

		SamplerState mySamplerState = SamplerState::eTriLinear;
		AdressMode myAdressMode = AdressMode::eClamp;
	};
}