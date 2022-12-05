#pragma once
#include "ISTE/Graphics/RenderStateEnums.h"
#include "ISTE/Graphics/RenderDefines.h"
#include "ISTE/ECSB/ECSDefines.hpp"
#include <ISTE/Math/Vec4.h> 
#include <ISTE/Math/Vec2.h>
#include <ISTE/Math/Matrix4x4.h>

namespace ISTE
{
	struct VFXSprite3DRenderCommand
	{
		bool operator<(VFXSprite3DRenderCommand& aLeft) { return myDiffLengthToCamera < aLeft.myDiffLengthToCamera; }

		//this is weird rethink this later
		CU::Matrix4x4f myTransform;
		CU::Vec4f myColor = { 1,1,1,1 };
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