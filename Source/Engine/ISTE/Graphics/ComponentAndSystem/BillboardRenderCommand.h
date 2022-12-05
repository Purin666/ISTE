#pragma once
#include "ISTE/Math/Vec.h"
#include "ISTE/Graphics/RenderDefines.h"
#include "ISTE/ECSB/ECSDefines.hpp"

namespace ISTE {
	struct BillboardCommand {
		CU::Matrix4x4f myTransform;
		
		CU::Vec4f	myColor			= CU::Vec4f(1,1,1,1);
		EntityIndex myEntityIndex	= EntityIndex(-1);

		TextureID	myTextures		= TextureID(-1);
		CU::Vec2f	myUVStart		= CU::Vec2f(0, 0);	//x:left	y:top
		CU::Vec2f	myUVEnd			= CU::Vec2f(1, 1);	//x:right	y:bottom
		CU::Vec2f	myUVOffset		= CU::Vec2f(0, 0);
		CU::Vec2f	myUVScale		= CU::Vec2f(1, 1);
	
		ISTE::SamplerState mySamplerState = ISTE::SamplerState::eTriLinear;
		ISTE::AdressMode myAdressMode = ISTE::AdressMode::eWrap;
	};

	struct BatchedBillboardCommand {
		CU::Matrix4x4f	myTransform;
		CU::Vec4f		myColor		= CU::Vec4f(1, 1, 1, 1);
		CU::Vec2f		myUVStart	= CU::Vec2f(0, 0);	//x:left	y:top
		CU::Vec2f		myUVEnd		= CU::Vec2f(1, 1);	//x:right	y:bottom
		CU::Vec2f		myUVOffset	= CU::Vec2f(0, 0);
		CU::Vec2f		myUVScale	= CU::Vec2f(1, 1);  
		EntityIndex		myEntityId  = EntityIndex(-1);
	};
}