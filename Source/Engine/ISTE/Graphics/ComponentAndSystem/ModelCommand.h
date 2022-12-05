#pragma once
#include <queue>
#include "ISTE/Graphics/RenderStateEnums.h"
#include "ISTE/Graphics/RenderDefines.h"
#include <ISTE/Math/Vec4.h> 
#include <ISTE/Math/Vec2.h>
#include <ISTE/Math/Matrix4x4.h>
#include <ISTE/ECSB/ECSDefines.hpp>
		  
namespace ISTE
{
	struct ModMapping
	{
		size_t		myMeshCount;
		uint64_t	myMapKey[MAX_MESH_COUNT];
		size_t		myCommandIndex[MAX_MESH_COUNT];
	};
	struct MeshMapping
	{
		uint64_t myMapKey;
		size_t myCommandIndex;
	};

	struct ModelCommand
	{
		CU::Matrix4x4f myTransform;
		CU::Vec4f myColor;
		CU::Vec2f myUV;
		CU::Vec2f myUVScale;

		int myMeshIndex; 
		ModelID myModelId							= ModelID(-1);
		TextureID myTextureIDs[MAX_MATERIAL_COUNT]	= { TextureID(-1) };
		EntityIndex myEntityId;

		ISTE::SamplerState mySamplerState = ISTE::SamplerState::eTriLinear;
		AdressMode myAdressMode = AdressMode::eClamp;
		RenderFlags myRenderFlags;
	};


	struct BatchedModelCommand {
		CU::Matrix4x4f	myTransform;
		CU::Vec4f		myColor;
		CU::Vec2f		myUVOffset;
		CU::Vec2f		myUVScale; 
		EntityIndex		myEntityId;
		RenderFlags		myRenderFlags;
		bool			myActive = true;
	};
	struct ModelBatchValue {
		~ModelBatchValue() = default;
		std::vector<BatchedModelCommand>	myBatchedCommands;
		int									myMeshID;
		ModelID								myModelID;
		TextureID							myTextureID[3];
	};

	struct ModelBatchKey {
		int			myMeshID;
		ModelID		myModelID;
		TextureID	myTextureID[3];
	};
};