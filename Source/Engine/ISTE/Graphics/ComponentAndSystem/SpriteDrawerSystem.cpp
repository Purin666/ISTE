#include "SpriteDrawerSystem.h"
#include <d3d11.h>
#include <fstream>

#include <ISTE/Context.h>
#include <ISTE/Graphics/DX11.h>

#include <ISTE/Scene/SceneHandler.h>
#include <ISTE/Scene/Scene.h>
#include <ISTE/Graphics/ComponentAndSystem/TransformComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/Sprite3DCommand.h>
#include <ISTE/Graphics/ComponentAndSystem/Sprite2DCommand.h>
#include <ISTE/Graphics/ComponentAndSystem/Sprite3DComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/Sprite2DComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/BillboardComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/MaterialComponent.h>
#include <ISTE/Graphics/GraphicsEngine.h>
#include <ISTE/Graphics/Resources/ModelManager.h>
#include <ISTE/Graphics/Resources/TextureManager.h>
#include <ISTE/Graphics/Resources/ShaderManager.h>
#include "ISTE/Graphics/RenderStateManager.h"
#include "ISTE/WindowsWindow.h"

struct CommonGBuffer {
	CU::Vec3f		myColor;
	int				myIndex;
}; 

struct BillboardBuffer {
	CU::Matrix4x4f	myModelToWorld;

	float			myEntityIndex; 
	CU::Vec3f		garb;

	CU::Vec4f		myColor;

	CU::Vec2f		myUVStart;
	CU::Vec2f		myUVEnd;
	CU::Vec2f		myUVOffset;
	CU::Vec2f		myUVScale;
};

struct Sprite3DObjectBuffer {
	CU::Matrix4x4f	myTransformMatrix;

	float	myEntityIndex;
	CU::Vec3f	garb;

	CU::Vec4f		myColor;

	CU::Vec2f		myUVStart;
	CU::Vec2f		myUVEnd;
	CU::Vec2f		myUVOffset;
	CU::Vec2f		myUVScale;
};
struct Sprite2DObjectBuffer {
	CU::Vec2f		myPosition; 
	CU::Vec2f		myScale; 
	
	float			myRot;
	float			garb;
	CU::Vec2f		myPivot;

	CU::Vec2f		myUVStart;
	CU::Vec2f		myUVEnd;
	CU::Vec2f		myUVOffset;
	CU::Vec2f		myUVScale;

	CU::Vec4f		myColor;
};

ISTE::SpriteDrawerSystem::~SpriteDrawerSystem()
{
}

bool ISTE::SpriteDrawerSystem::Init()
{
	myCtx = Context::Get();
	mySceneHandler = myCtx->mySceneHandler;
	mySpriteQuad = myCtx->myModelManager->LoadSpriteQuad(); 

	if (!InitShader("Shaders/3DSprite_VS.cso", "Shaders/3DSprite_PS.cso",SpriteComplexity::e3DSprite))
		return false;
	if (!InitShader("Shaders/2DSprite_VS.cso", "Shaders/2DSprite_PS.cso", SpriteComplexity::e2DSprite))
		return false;
	if (!InitShader("Shaders/Billboard_VS.cso", "Shaders/Billboard_PS.cso", SpriteComplexity::eBillBoard))
		return false;

	if (!Init2DSpriteBuffer())
		return false;
	if (!Init3DSpriteBuffer())
		return false;
	if (!InitBillboardBuffer())
		return false;
	if (!InitCommonGBuffer())
		return false;
	return true;
}

void ISTE::SpriteDrawerSystem::PrepareRenderCommands()
{
	Prep3DSprites();
	Prep2DSprites();
	PrepBillboards();
}

void ISTE::SpriteDrawerSystem::Draw()
{
	myCtx->myModelManager->BindMesh(mySpriteQuad,0);
	Render2DSprites();
	Render3DSprites();
	RenderBillboards();
}

void ISTE::SpriteDrawerSystem::DrawDepth()
{
	Render2DSpriteDepth();
	Render3DSpriteDepth();
	RenderBillboardDepth();

}

void ISTE::SpriteDrawerSystem::ClearCommands()
{
	size_t size = my2DRenderCommands.size();
	my2DRenderCommands.clear();
	my2DRenderCommands.reserve(size);

	size = my3DRenderCommands.size();
	my3DRenderCommands.clear();
	my3DRenderCommands.reserve(size);

	size = myBillboardCommands.size();
	myBillboardCommands.clear();
	myBillboardCommands.reserve(size);
}

void ISTE::SpriteDrawerSystem::Add2DSpriteRenderCommand(const Sprite2DRenderCommand& aCmd)
{
	my2DRenderCommands.emplace_back(aCmd);
}

void ISTE::SpriteDrawerSystem::Add3DSpriteRenderCommand(const Sprite3DRenderCommand& aCmd)
{
	my3DRenderCommands.emplace_back(aCmd);
}

void ISTE::SpriteDrawerSystem::AddBillboardCommand(const BillboardCommand& aCmd)
{
	myBillboardCommands.emplace_back(aCmd);
}

void ISTE::SpriteDrawerSystem::Render2DSprites()
{
	myCtx->myRenderStateManager->SetBlendState(ISTE::BlendState::eAlphaBlend);

	myCtx->myModelManager->BindMesh(mySpriteQuad, 0);
	BindShader(SpriteComplexity::e2DSprite);

	size_t size = my2DRenderCommands.size();
	Sprite2DRenderCommand com;
	for (int i = 0; i < size; i++)
	{
		com = my2DRenderCommands[i];

		//binds obj buffer
		BindSprite2DObjBuffer(com);

		//sampler state
		myCtx->myRenderStateManager->SetSampleState(com.myAdressMode, com.mySamplerState);

		//sets texture
		myCtx->myTextureManager->PsBindTexture(com.myTextureId[ALBEDO_MAP], 0);
		//myCtx->myTextureManager->PsBindTexture(com.myTextureId, 0);
		//myCtx->myTextureManager->PsBindTexture(com.myTextureId, 0);

		//end stuff
		myCtx->myModelManager->RenderMesh(mySpriteQuad, 0);
	}
}

void ISTE::SpriteDrawerSystem::Render3DSprites()
{
	myCtx->myModelManager->BindMesh(mySpriteQuad, 0);
	BindShader(SpriteComplexity::e3DSprite);

	size_t size = my3DRenderCommands.size();
	Sprite3DRenderCommand com;
	for (int i = 0; i < size; i++)
	{
		com = my3DRenderCommands[i];

		//binds obj buffer
		BindSprite3DObjBuffer(com);

		//sampler state
		myCtx->myRenderStateManager->SetSampleState(com.myAdressMode, com.mySamplerState);

		//sets texture
		myCtx->myTextureManager->PsBindTexture(com.myTextureId, 0);

		//end stuff
		myCtx->myModelManager->RenderMesh(mySpriteQuad, 0);
	}
}

void ISTE::SpriteDrawerSystem::RenderBillboards()
{
	BindShader(SpriteComplexity::eBillBoard);
	myCtx->myModelManager->BindMesh(mySpriteQuad, 0);

	BillboardCommand com;
	size_t size = myBillboardCommands.size();
	for (int i = 0; i < size; i++)
	{
		com = myBillboardCommands[i];

		//binds obj buffer
		BindBillBoardBuffer(com);

		//sampler state
		myCtx->myRenderStateManager->SetSampleState(com.myAdressMode, com.mySamplerState);

		//sets texture
		myCtx->myTextureManager->PsBindTexture(com.myTextures, 0);

		//end stuff
		myCtx->myModelManager->RenderMesh(mySpriteQuad, 0);
	}
}

void ISTE::SpriteDrawerSystem::AddBatchedBillboardCommand(const BillboardCommand& aCom) 
{

	BatchedBillboardCommand bbc;
	bbc.myColor		= aCom.myColor;
	bbc.myEntityId	= aCom.myEntityIndex;
	bbc.myTransform = aCom.myTransform;
	bbc.myUVEnd		= aCom.myUVEnd;
	bbc.myUVStart	= aCom.myUVStart;
	bbc.myUVScale	= aCom.myUVScale;
	bbc.myUVOffset	= aCom.myUVOffset;

	auto& value = myBatchedBillboardCommands.find(aCom.myTextures);
	if (value == myBatchedBillboardCommands.end())
	{
		BillboardBatchValue MBvalue;
		MBvalue.myBatchedCommands = (BatchedBillboardCommand*)malloc(sizeof(BatchedBillboardCommand) * MODEL_INSTANCE_BATCH_SIZE);
		MBvalue.myBatchedCommands[0] = bbc;
		MBvalue.myBatchSize = 1;
		MBvalue.myTextureID = aCom.myTextures;
		myBatchedBillboardCommands[aCom.myTextures] = MBvalue;
	}
	else
	{
		value->second.myBatchedCommands[value->second.myBatchSize] = bbc;
		value->second.myBatchSize++;
	}
}

void ISTE::SpriteDrawerSystem::Render2DSpriteDepth()
{
	myCtx->myModelManager->BindMesh(mySpriteQuad, 0);
	myCtx->myDX11->GetContext()->VSSetShader(myVertexShader[(int)SpriteComplexity::e2DSprite].Get(), 0, 0);
	myCtx->myDX11->GetContext()->IASetInputLayout(myInputLayout[(int)SpriteComplexity::e2DSprite].Get()); 
	myCtx->myDX11->GetContext()->PSSetShader(nullptr, 0, 0);

	Sprite2DRenderCommand com;
	size_t size = my2DRenderCommands.size();
	for (int i = 0; i < size; i++)
	{
		com = my2DRenderCommands[i];

		//binds obj buffer
		BindSprite2DObjBuffer(com);

		//sampler state
		myCtx->myRenderStateManager->SetSampleState(com.myAdressMode, com.mySamplerState);

		//sets texture
		myCtx->myTextureManager->PsBindTexture(com.myTextureId[ALBEDO_MAP], 0);

		//end stuff
		myCtx->myModelManager->RenderMesh(mySpriteQuad, 0);
	}
}

void ISTE::SpriteDrawerSystem::Render3DSpriteDepth()
{
	myCtx->myModelManager->BindMesh(mySpriteQuad, 0); 
	myCtx->myDX11->GetContext()->VSSetShader(myVertexShader[(int)SpriteComplexity::e3DSprite].Get(), 0, 0);
	myCtx->myDX11->GetContext()->IASetInputLayout(myInputLayout[(int)SpriteComplexity::e3DSprite].Get());
	myCtx->myDX11->GetContext()->PSSetShader(nullptr, 0, 0);

	size_t size = my3DRenderCommands.size();
	Sprite3DRenderCommand com;
	for (int i = 0; i < size; i++)
	{
		com = my3DRenderCommands[i];

		//binds obj buffer
		BindSprite3DObjBuffer(com);

		//sampler state
		myCtx->myRenderStateManager->SetSampleState(com.myAdressMode, com.mySamplerState);

		//sets texture
		myCtx->myTextureManager->PsBindTexture(com.myTextureId, 0);

		//end stuff
		myCtx->myModelManager->RenderMesh(mySpriteQuad, 0);
	}
}

void ISTE::SpriteDrawerSystem::RenderBillboardDepth()
{
	myCtx->myDX11->GetContext()->VSSetShader(myVertexShader[(int)SpriteComplexity::eBillBoard].Get(), 0, 0);
	myCtx->myDX11->GetContext()->IASetInputLayout(myInputLayout[(int)SpriteComplexity::eBillBoard].Get());
	myCtx->myDX11->GetContext()->PSSetShader(nullptr, 0, 0);
	myCtx->myModelManager->BindMesh(mySpriteQuad, 0);

	BillboardCommand com;
	size_t size = myBillboardCommands.size();
	for (int i = 0; i < size; i++)
	{
		com = myBillboardCommands[i];

		//binds obj buffer
		BindBillBoardBuffer(com);

		//sampler state
		myCtx->myRenderStateManager->SetSampleState(com.myAdressMode, com.mySamplerState);

		//sets texture
		myCtx->myTextureManager->PsBindTexture(com.myTextures, 0);

		//end stuff
		myCtx->myModelManager->RenderMesh(mySpriteQuad, 0);
	}

}

void ISTE::SpriteDrawerSystem::RenderBatchedBillboardDepth()
{
	//its not done yet

	//InstanceBuffer* instanceBufferData;
	//CommonGBuffer* instanceGBuffer;
	//
	//size_t i;
	//size_t batchSize;
	//unsigned int offset[2] = { 0, 0 };
	//unsigned int strides[2] = { sizeof(VertexData), sizeof(InstanceBuffer) };
	//
	//for (auto& element : myBatchedRenderCommands)
	//{
	//	commandBatch = element.second.myBatchedCommands;
	//	curModel = modelList.GetByIterator(element.second.myModelID);
	//	textureIDs = element.second.myTextureID;
	//	meshID = element.second.myMeshID;
	//	size = element.second.myBatchSize;
	//
	//	i = 0;
	//	while (i < size)
	//	{
	//		dCtx->Map(myInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &instanceBufferMappedData);
	//		dCtx->Map(myCommonGBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gBufferMappedData);
	//
	//		instanceBufferData = (InstanceBuffer*)instanceBufferMappedData.pData;
	//		instanceGBuffer = (CommonGBuffer*)gBufferMappedData.pData;
	//		batchSize = 0;
	//		for (; batchSize < ModelInstanceBatchSize && i < size; batchSize++)
	//		{
	//			mRC = commandBatch[i];
	//			instanceBufferData[batchSize].myTransform = mRC.myTransform;
	//			instanceGBuffer->myInstanceData[batchSize].myIndex = mRC.myEntityId;
	//			instanceGBuffer->myInstanceData[batchSize].myColor = mRC.myColor;
	//			Logger::LoggBatchedDrawCall();
	//			i++;
	//		}
	//		dCtx->Unmap(myInstanceBuffer.Get(), 0);
	//		dCtx->Unmap(myCommonGBuffer.Get(), 0);
	//
	//		myCtx->myTextureManager->PsBindTexture(textureIDs[AlbedoMap], 0);
	//		myCtx->myTextureManager->PsBindTexture(textureIDs[NormalMap], 1);
	//		myCtx->myTextureManager->PsBindTexture(textureIDs[MaterialMap], 2);
	//
	//		dCtx->PSSetConstantBuffers(2, 1, myCommonGBuffer.GetAddressOf());
	//
	//		Mesh& curMesh = curModel->myMeshes[meshID];
	//		curMesh.BindIndexBuffer();
	//
	//		ID3D11Buffer* buffers[2] = { curMesh.GetVertexBuffer(), myInstanceBuffer.Get() };
	//
	//		dCtx->IASetVertexBuffers(0, 2, buffers, strides, offset);
	//		dCtx->DrawIndexedInstanced(curMesh.GetIndexCount(), batchSize, 0, 0, 0);
	//		Logger::LoggDrawCall();
	//	}
	//}
}

void ISTE::SpriteDrawerSystem::Prep3DSprites()
{
	Scene& scene = mySceneHandler->GetActiveScene();
	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
	ComponentPool& sprite3DPool = scene.GetComponentPool<Sprite3DComponent>();

	Sprite3DRenderCommand SpriteRenderCommand;
	Sprite3DComponent* spriteData;
	TransformComponent* transformData;
	for (auto entity : myEntities[(int)SpriteComplexity::e3DSprite])
	{
		SpriteRenderCommand.myEntityIndex = GetEntityIndex(entity);
		//processes sprite data
		{
			spriteData = (Sprite3DComponent*)sprite3DPool.Get(SpriteRenderCommand.myEntityIndex);
			SpriteRenderCommand.myTextureId		= spriteData->myTextureId;
			SpriteRenderCommand.myColor			= { spriteData->myColor.x,spriteData->myColor.y,spriteData->myColor.z,1 };
			SpriteRenderCommand.myUVStart		= spriteData->myUVStart;
			SpriteRenderCommand.myUVEnd			= spriteData->myUVEnd;
			SpriteRenderCommand.myUVOffset		= spriteData->myUVOffset;
			SpriteRenderCommand.myUVScale		= spriteData->myUVScale;
			SpriteRenderCommand.mySamplerState	= spriteData->mySamplerState;
			SpriteRenderCommand.myAdressMode	= spriteData->myAdressMode;
		}

		//processses transform data
		{
			transformData = (TransformComponent*)transformPool.Get(SpriteRenderCommand.myEntityIndex);
			SpriteRenderCommand.myTransform = transformData->myCachedTransform;
		}
		my3DRenderCommands.emplace_back(SpriteRenderCommand);
	}
}

void ISTE::SpriteDrawerSystem::Prep2DSprites()
{
	Scene& scene = mySceneHandler->GetActiveScene();
	ComponentPool& sprite2DPool = scene.GetComponentPool<Sprite2DComponent>();
	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
	ComponentPool& matPool = scene.GetComponentPool<MaterialComponent>();
	
	Sprite2DRenderCommand SpriteRenderCommand;
	MaterialComponent* matData;
	Sprite2DComponent* spriteData;
	TransformComponent* transformData;
	for (auto entity : myEntities[(int)SpriteComplexity::e2DSprite])
	{
		SpriteRenderCommand.myEntityIndex = GetEntityIndex(entity);
		//processes sprite data
		//material
		{
		matData = (MaterialComponent*)matPool.Get(GetEntityIndex(entity));
		if (matData->myTextures[0][ALBEDO_MAP] == TextureID(-1))
			continue;
		memcpy(SpriteRenderCommand.myTextureId, matData->myTextures, sizeof(TextureID) * 3);
		}

		{
		spriteData = (Sprite2DComponent*)sprite2DPool.Get(GetEntityIndex(entity)); 

		SpriteRenderCommand.myColor			= { spriteData->myColor.x,spriteData->myColor.y,spriteData->myColor.z,1 };
		SpriteRenderCommand.myUVStart		= spriteData->myUVStart;
		SpriteRenderCommand.myUVEnd			= spriteData->myUVEnd;
		SpriteRenderCommand.myUVOffset		= spriteData->myUVOffset;
		SpriteRenderCommand.myUVScale		= spriteData->myUVScale;
		SpriteRenderCommand.mySamplerState	= spriteData->mySamplerState;
		SpriteRenderCommand.myAdressMode	= spriteData->myAdressMode;
		}


		//processses transform data
		{
		transformData = (TransformComponent*)transformPool.Get(GetEntityIndex(entity));
		SpriteRenderCommand.myPosition.x	= transformData->myPosition.x;
		SpriteRenderCommand.myPosition.y	= transformData->myPosition.y;

		SpriteRenderCommand.myScale.x		= transformData->myScale.x;
		SpriteRenderCommand.myScale.y		= transformData->myScale.y;

		SpriteRenderCommand.myRotation		= transformData->myEuler.GetAngles().z; 
		}
		my2DRenderCommands.emplace_back(SpriteRenderCommand);
	}
}

void ISTE::SpriteDrawerSystem::PrepBatched3DSprites()
{
}

void ISTE::SpriteDrawerSystem::PrepBatched2DSprites()
{


}

void ISTE::SpriteDrawerSystem::PrepBatchedBillboards()
{
	Scene& scene = mySceneHandler->GetActiveScene();
	ModelManager* modelManager = myCtx->myModelManager;
	ComponentPool& billboardpool	= scene.GetComponentPool<BillboardComponent>();
	ComponentPool& transformPool	= scene.GetComponentPool<TransformComponent>();
	ComponentPool& matPool			= scene.GetComponentPool<MaterialComponent>();

	BatchedBillboardCommand	mC;
	BillboardComponent*		spriteData;
	MaterialComponent*		matData;
	TransformComponent*		transformData;
	EntityID				entID;
	TextureID				texID;
	Model*					model;
	size_t					size; 
	BillboardBatchValue		MBvalue = {};

	for (auto entity : myEntities[0])
	{
		entID = mC.myEntityId = GetEntityIndex(entity);
		spriteData = (BillboardComponent*)billboardpool.Get(entID);
		{
			mC.myColor		= { spriteData->myColor.x,spriteData->myColor.y,spriteData->myColor.z,1 };
			mC.myUVEnd		= spriteData->myUVEnd;
			mC.myUVStart	= spriteData->myUVStart; 
			mC.myUVScale	= spriteData->myUVScale;
			mC.myUVOffset	= spriteData->myUVOffset;
		}

		matData		= (MaterialComponent*)matPool.Get(entID); 
		texID		= matData->myTextures[0][ALBEDO_MAP];//assumes only albedo is assigned

		//processses transform data 
		transformData	= (TransformComponent*)transformPool.Get(entID);
		mC.myTransform	= transformData->myCachedTransform; 

		auto& value = myBatchedBillboardCommands.find(texID);
		if (value == myBatchedBillboardCommands.end())
		{ 
			MBvalue.myBatchedCommands = (BatchedBillboardCommand*)malloc(sizeof(BatchedBillboardCommand) * MODEL_INSTANCE_BATCH_SIZE);
			MBvalue.myBatchedCommands[0] = mC;
			MBvalue.myBatchSize = 1;
			MBvalue.myTextureID = texID; 
			myBatchedBillboardCommands[texID] = MBvalue;
		}
		else
		{
			value->second.myBatchedCommands[value->second.myBatchSize] = mC;
			value->second.myBatchSize++;
		}
		
	}
}

void ISTE::SpriteDrawerSystem::PrepBillboards()
{
	Scene& scene = mySceneHandler->GetActiveScene();
	ComponentPool& billboardComponent	= scene.GetComponentPool<BillboardComponent>();
	ComponentPool& transformPool		= scene.GetComponentPool<TransformComponent>();
	ComponentPool& matPool				= scene.GetComponentPool<MaterialComponent>();
	
	BillboardCommand billboardCommand;
	BillboardComponent* spriteData;
	MaterialComponent* matData;
	TransformComponent* transformData;
	for (auto entity : myEntities[(int)SpriteComplexity::eBillBoard])
	{
		billboardCommand.myEntityIndex = GetEntityIndex(entity);
		//processes sprite data
		//material
		{
		matData = (MaterialComponent*)matPool.Get(GetEntityIndex(entity));
		if (matData->myTextures[0][ALBEDO_MAP] == TextureID(-1))
			continue;
		billboardCommand.myTextures = matData->myTextures[0][0];
		}

		{
		spriteData = (BillboardComponent*)billboardComponent.Get(GetEntityIndex(entity));

		billboardCommand.myColor		= { spriteData->myColor.x,spriteData->myColor.y,spriteData->myColor.z,1 };
		billboardCommand.myUVStart		= spriteData->myUVStart;
		billboardCommand.myUVEnd		= spriteData->myUVEnd;
		billboardCommand.myUVOffset		= spriteData->myUVOffset;
		billboardCommand.myUVScale		= spriteData->myUVScale; 
		billboardCommand.myAdressMode	= spriteData->myAdressMode;
		billboardCommand.mySamplerState	= spriteData->mySamplerState;
		}


		//processses transform data
		{
		transformData = (TransformComponent*)transformPool.Get(GetEntityIndex(entity));
		billboardCommand.myTransform	  = transformData->myCachedTransform; 
		}
		myBillboardCommands.emplace_back(billboardCommand);
	}
}

void ISTE::SpriteDrawerSystem::BindSprite3DObjBuffer(const Sprite3DRenderCommand& aCommand)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	HRESULT res = context->Map(myObjBuffer[(int)SpriteComplexity::e3DSprite].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	Sprite3DObjectBuffer* OBdata = (Sprite3DObjectBuffer*)(resource.pData);

	OBdata->myTransformMatrix = aCommand.myTransform; 

	OBdata->myEntityIndex	= aCommand.myEntityIndex;
	OBdata->myColor			= aCommand.myColor;
	OBdata->myUVOffset		= aCommand.myUVOffset;
	OBdata->myUVStart		= aCommand.myUVStart;
	OBdata->myUVEnd			= aCommand.myUVEnd;
	OBdata->myUVScale		= aCommand.myUVScale;

	context->Unmap(myObjBuffer[(int)SpriteComplexity::e3DSprite].Get(), 0);

	context->VSSetConstantBuffers(2, 1, myObjBuffer[(int)SpriteComplexity::e3DSprite].GetAddressOf());
	context->PSSetConstantBuffers(2, 1, myObjBuffer[(int)SpriteComplexity::e3DSprite].GetAddressOf()); 
}

void ISTE::SpriteDrawerSystem::BindSprite2DObjBuffer(const Sprite2DRenderCommand& aCommand)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	HRESULT res = context->Map(myObjBuffer[(int)SpriteComplexity::e2DSprite].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	Sprite2DObjectBuffer* OBdata = (Sprite2DObjectBuffer*)(resource.pData); 
	OBdata->myPosition	= aCommand.myPosition;
	OBdata->myRot		= aCommand.myRotation;
	OBdata->myScale		= aCommand.myScale; 
	OBdata->myPivot		= aCommand.myPivot;
	OBdata->myUVStart	= aCommand.myUVStart;
	OBdata->myUVEnd		= aCommand.myUVEnd;
	OBdata->myUVOffset	= aCommand.myUVOffset;
	OBdata->myUVScale	= aCommand.myUVScale;
	OBdata->myColor		= aCommand.myColor;
	context->Unmap(myObjBuffer[(int)SpriteComplexity::e2DSprite].Get(), 0);

	context->VSSetConstantBuffers(2, 1, myObjBuffer[(int)SpriteComplexity::e2DSprite].GetAddressOf());
	context->PSSetConstantBuffers(2, 1, myObjBuffer[(int)SpriteComplexity::e2DSprite].GetAddressOf()); 
}

void ISTE::SpriteDrawerSystem::BindBillBoardBuffer(const BillboardCommand& aCommand)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	HRESULT res = context->Map(myObjBuffer[(int)SpriteComplexity::eBillBoard].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	BillboardBuffer* OBdata = (BillboardBuffer*)(resource.pData);
	
	OBdata->myModelToWorld	= aCommand.myTransform;
	OBdata->myEntityIndex   = aCommand.myEntityIndex;
	OBdata->myUVOffset		= aCommand.myUVOffset;
	OBdata->myUVStart		= aCommand.myUVStart;
	OBdata->myUVEnd			= aCommand.myUVEnd;
	OBdata->myUVScale		= aCommand.myUVScale; 
	OBdata->myColor			= aCommand.myColor;  

	context->Unmap(myObjBuffer[(int)SpriteComplexity::eBillBoard].Get(), 0);
	 

	context->VSSetConstantBuffers(2, 1, myObjBuffer[(int)SpriteComplexity::eBillBoard].GetAddressOf());
	context->PSSetConstantBuffers(2, 1, myObjBuffer[(int)SpriteComplexity::eBillBoard].GetAddressOf());
}

void ISTE::SpriteDrawerSystem::BindShader(SpriteComplexity aComplexity)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetShader(myVertexShader[(int)aComplexity].Get(), 0, 0);
	context->PSSetShader(myPixelShader[(int)aComplexity].Get(), 0, 0);
	context->IASetInputLayout(myInputLayout[(int)aComplexity].Get());
}

bool ISTE::SpriteDrawerSystem::InitShader(std::string aVSPath, std::string aPSPath, SpriteComplexity aLightComplexity)
{
	ID3D11Device* aDevice = myCtx->myDX11->GetDevice();
	HRESULT result;
	std::string vsData;
	DX11::CreateVertexShader(aVSPath, myVertexShader[(int)aLightComplexity].GetAddressOf(), vsData);
	DX11::CreatePixelShader(aPSPath, myPixelShader[(int)aLightComplexity].GetAddressOf());
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"UV",
				0,
				DXGI_FORMAT_R32G32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"NORMAL",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"BINORMAL",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"TANGENT",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"BONES",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"WEIGHTS",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
		};

		int elementCount = sizeof(layout) / sizeof(layout[0]);
		result = aDevice->CreateInputLayout(layout, elementCount, vsData.data(), vsData.size(), myInputLayout[(int)aLightComplexity].GetAddressOf());
		if (FAILED(result))
			return false;
	}
	return true;
}

bool ISTE::SpriteDrawerSystem::Init2DSpriteBuffer()
{
	D3D11_BUFFER_DESC objBufferDesc = {};
	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objBufferDesc.ByteWidth = sizeof(Sprite2DObjectBuffer);
	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myObjBuffer[(int)SpriteComplexity::e2DSprite].GetAddressOf());
	if (FAILED(result))
		return false;  

	return true;
}

bool ISTE::SpriteDrawerSystem::Init3DSpriteBuffer()
{
	D3D11_BUFFER_DESC objBufferDesc = {};
	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objBufferDesc.ByteWidth = sizeof(Sprite3DObjectBuffer);
	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myObjBuffer[(int)SpriteComplexity::e3DSprite].GetAddressOf());
	if (FAILED(result))
		return false;

	return true;
}

bool ISTE::SpriteDrawerSystem::InitBillboardBuffer()
{
	D3D11_BUFFER_DESC objBufferDesc = {};
	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objBufferDesc.ByteWidth = sizeof(BillboardBuffer);
	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myObjBuffer[(int)SpriteComplexity::eBillBoard].GetAddressOf());
	if (FAILED(result))
		return false;

	return true;
}

bool ISTE::SpriteDrawerSystem::InitCommonGBuffer()
{
	D3D11_BUFFER_DESC objBufferDesc = {};
	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objBufferDesc.ByteWidth = sizeof(CommonGBuffer);
	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myCommonGBuffer.GetAddressOf());
	if (FAILED(result))
		return false;

	return true;
}
