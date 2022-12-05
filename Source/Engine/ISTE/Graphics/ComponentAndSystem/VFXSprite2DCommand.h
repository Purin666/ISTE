#pragma once
#include "ISTE/Graphics/RenderStateEnums.h"
#include "ISTE/Graphics/RenderDefines.h"
#include "ISTE/ECSB/ECSDefines.hpp"
#include <ISTE/Math/Vec4.h> 
#include <ISTE/Math/Vec2.h>
#include <ISTE/Math/Matrix4x4.h>

namespace ISTE
{
	struct VFXSprite2DRenderCommand
	{
		bool operator<(VFXSprite2DRenderCommand& aLeft) { return myDiffLengthToCamera < aLeft.myDiffLengthToCamera; }
		
		CU::Vec2f myPosition;
		CU::Vec2f myScale;
		float myRotation;

		CU::Vec4f myColor;
		CU::Vec2f myUVStart;
		CU::Vec2f myUVEnd;
		
		CU::Vec2f myUVScale;
		CU::Vec2f myUVOffset;

		float myDiffLengthToCamera	= -1;
		TextureID myTextureId		= -1;
		EntityIndex myEntityIndex	= EntityIndex(-1);
		ISTE::SamplerState mySamplerState = ISTE::SamplerState::eTriLinear;
		ISTE::AdressMode myAdressMode = ISTE::AdressMode::eWrap;
	};
}