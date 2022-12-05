#pragma once
#include <ISTE/Graphics/RenderStateEnums.h>
#include <ISTE/Graphics/RenderDefines.h>
#include <ISTE/ECSB/ECSDefines.hpp>
#include <ISTE/Math/Vec4.h> 
#include <ISTE/Math/Vec2.h>
#include <ISTE/Math/Matrix4x4.h>

namespace ISTE
{
	struct Sprite2DRenderCommand
	{
		bool operator<(Sprite2DRenderCommand& aLeft) { return myDiffLengthToCamera < aLeft.myDiffLengthToCamera; }
		
		CU::Vec2f myPosition	= CU::Vec2f(0.f, 0.f);
		CU::Vec2f myScale		= CU::Vec2f(1.f, 1.f);
		float myRotation		= 0.f;
		CU::Vec2f myPivot		= CU::Vec2f(0.0f, 0.0f);

		CU::Vec4f myColor		= CU::Vec4f(1.f, 1.f, 1.f, 1.f);
		
		CU::Vec2f myUVStart		= CU::Vec2f(0.f, 0.f);
		CU::Vec2f myUVEnd		= CU::Vec2f(1.f, 1.f);
		CU::Vec2f myUVScale		= CU::Vec2f(1.f, 1.f);
		CU::Vec2f myUVOffset	= CU::Vec2f(0.f, 0.f);

		float myDiffLengthToCamera			= -1;
		TextureID myTextureId[3]			= { TextureID(-1),TextureID(-1),TextureID(-1) };
		EntityIndex myEntityIndex			= EntityIndex(-1);
		ISTE::SamplerState mySamplerState = ISTE::SamplerState::eTriLinear;
		ISTE::AdressMode myAdressMode = ISTE::AdressMode::eWrap;
	};
}