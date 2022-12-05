#pragma once
#include "ISTE/Graphics/RenderStateEnums.h"
#include <ISTE/Math/Vec4.h> 
#include <ISTE/Math/Vec2.h> 
#include <ISTE/Graphics/RenderDefines.h>

namespace ISTE
{
	struct ModelComponent
	{
		CU::Vec3f	myColor		= {1,1,1};
		CU::Vec2f	myUV		= {0,0};
		CU::Vec2f	myUVScale	= {1,1};

		ModelID		myModelId	= ModelID(-1);

		TextureID	myTextureIDs[3] = { TextureID(-1) };

		SamplerState mySamplerState	= SamplerState::eTriLinear;
		AdressMode	 myAdressMode	= AdressMode::eWrap;
	};
}