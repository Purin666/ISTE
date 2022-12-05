#pragma once
#include "ISTE/Graphics/RenderStateEnums.h"
#include <ISTE/Graphics/RenderDefines.h>
#include <ISTE/ECSB/ECSDefines.hpp>
#include <ISTE/Math/Vec4.h> 
#include <ISTE/Math/Vec2.h>
#include <ISTE/Math/Matrix4x4.h>


namespace ISTE
{
	struct AnimLightIndex
	{
		bool myActive;
		size_t myCommandIndex;
	};

	struct AnimationCommand
	{
		CU::Matrix4x4f myTransform;
		CU::Matrix4x4f myPrevTransform;
		CU::Matrix4x4f myBindPoseInverse[MAX_BONE_COUNT];

		CU::Vec4f myColor;
		CU::Vec2f myUV;
		CU::Vec2f myUVScale;

		size_t myMeshCount = 0;

		ModelID myModelId = ModelID(-1);
		TextureID myTextureIDs[MAX_MESH_COUNT][MAX_MATERIAL_COUNT];
		EntityIndex myEntityIndex = EntityIndex(-1);

		RenderFlags myRenderFlags;

		ISTE::SamplerState mySamplerState = ISTE::SamplerState::eTriLinear;
		AdressMode myAdressMode = AdressMode::eClamp;
		bool myActive = true;
	};
};