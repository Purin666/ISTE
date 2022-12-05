#include "ModelDrawerSystem.h"
#include <d3d11.h>
#include <fstream>

#include <ISTE/Graphics/ComponentAndSystem/TransformComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/ModelComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/CustomShaderComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/MaterialComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h>
#include <ISTE/ECSB/ComponentPool.h>
#include "ISTE/Graphics/RenderStateManager.h"

#include <ISTE/Context.h>
#include <ISTE/Graphics/DX11.h>
#include <ISTE/Scene/SceneHandler.h>
#include <ISTE/Scene/Scene.h>
#include <ISTE/Graphics/Resources/TextureManager.h>
#include <ISTE/Graphics/Resources/ModelManager.h>
#include <ISTE/Graphics/Resources/ShaderManager.h>

#include <ISTE/Logger/Logger.h>
#include <Hash/MurmurHash2.hpp>

struct CommonGBuffer {
	struct InstanceData {
	CU::Vec3f		myColor;
	unsigned int	myIndex;
	float			myRenderFlags;
	CU::Vec3f		myGHarb;
	} myInstanceData[MODEL_INSTANCE_BATCH_SIZE];
};

struct InstanceBuffer {
	CU::Matrix4x4f myTransform;
	CU::Vec4f myUvScaleAndOffset;
};

ISTE::ModelDrawerSystem::~ModelDrawerSystem()
{
}

bool ISTE::ModelDrawerSystem::Init(const std::string& aVSPath, const std::string& aPSPath)
{  
	myCtx = Context::Get();
	mySceneHandler = myCtx->mySceneHandler;
	mySystemManager = myCtx->mySystemManager;
	if (!InitShader(aVSPath, aPSPath))
		return false;
	if (!InitBuffers())
		return false;
	
	myRenderCommands.reserve(MAX_ENTITIES);

	return true;
}

void ISTE::ModelDrawerSystem::AddRenderCommand(ModelCommand aRC)
{  
	myRenderCommands.push_back(aRC);
}

void ISTE::ModelDrawerSystem::PrepareBatchedRenderCommands()
{
	Scene& scene					= mySceneHandler->GetActiveScene();
	ModelManager* modelManager		= myCtx->myModelManager;
	ComponentPool& modelPool		= scene.GetComponentPool<ModelComponent>();
	ComponentPool& materialPool		= scene.GetComponentPool<MaterialComponent>();
	ComponentPool& transformPool	= scene.GetComponentPool<TransformComponent>();

	BatchedModelCommand mC;
	ModelComponent* modelData;
	MaterialComponent* materialData;
	TransformComponent* transformData;
	ModelID modelID;
	TextureID texID[3];
	Model* model;
	size_t size;
	uint64_t key;
	ModelBatchValue MBvalue = {};
	ModelBatchValue* ASS;

	unsigned int mod;
	unsigned int tex2;
	unsigned int tex;
	unsigned int tex3;

	unsigned int hash[3];

	for (auto& entity : myEntities[0])
	{ 
		modelData = (ModelComponent*)modelPool.Get(GetEntityIndex(entity)); 
		modelID = modelData->myModelId;
		{
		mC.myColor			= { modelData->myColor.x,modelData->myColor.y,modelData->myColor.z,1 };
		mC.myUVOffset		= modelData->myUV;
		mC.myUVScale		= modelData->myUVScale; 
		mC.myEntityId		= GetEntityIndex(entity); 
		if (myCtx->myModelManager->GetModelType(modelID) == ModelType::eTransparantModel)
		{
			auto* comp = scene.AssignComponent<CustomShaderComponent>(entity);
			comp->myShader = Shaders::eDefaultModelShader;
		}
		}
		 
		materialData = (MaterialComponent*)materialPool.Get(GetEntityIndex(entity));
		mC.myRenderFlags = materialData->myRenderFlags;

		//processses transform data 
		transformData = (TransformComponent*)transformPool.Get(GetEntityIndex(entity));
		mC.myTransform = transformData->myCachedTransform; 
		//char* mod = (char*)&mC.myModelId;
		//char* tex1 = (char*)&mC.myTextureIDs[0];
		//char* tex2 = (char*)&mC.myTextureIDs[1];
		//char* tex3 = (char*)&mC.myTextureIDs[2];

		if (modelID == ModelID(-1))
			continue;

		model = myCtx->myModelManager->GetModel(modelID);
		size = model->myMeshCount; 

		
		for (size_t meshIndex = 0; meshIndex < size; meshIndex++)
		{
			memcpy(texID, materialData->myTextures[meshIndex], sizeof(TextureID) * MAX_MATERIAL_COUNT);

			mod =  (unsigned int)modelID << 16;
			tex2 = (unsigned int)texID[1] << 16;
			tex =  (unsigned int)texID[0];
			tex3 = (unsigned int)texID[2];

			hash[0] = mod | tex;
			hash[1] = tex2 | tex3;
			hash[2] = meshIndex;
			key = MurmurHash::MurmurHash2(hash, sizeof(int) * 3, 0);

			auto& value = myBatchedRenderCommands.find(key);
			if (value == myBatchedRenderCommands.end())
			{ 
				MBvalue.myModelID				= modelID;
				MBvalue.myMeshID				= meshIndex; 
				MBvalue.myTextureID[0]			= texID[0];
				MBvalue.myTextureID[1]			= texID[1];
				MBvalue.myTextureID[2]			= texID[2];
				myBatchedRenderCommands[key]	= MBvalue;
				myBatchedRenderCommands[key].myBatchedCommands.reserve(MODEL_INSTANCE_BATCH_SIZE);
				myBatchedRenderCommands[key].myBatchedCommands.emplace_back(mC);		//LKJBDAÖKJBDA
			}
			else
			{
				value->second.myBatchedCommands.emplace_back(mC);
			}
		}
	}
}

void ISTE::ModelDrawerSystem::PrepareRenderCommands()
{
	Scene& scene = mySceneHandler->GetActiveScene();
	ComponentPool& modelPool = scene.GetComponentPool<ModelComponent>();
	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();

	for (auto entity : myEntities[0])
	{
		ModelCommand ModelCommand;
		//processes model data
		{ 
		ModelComponent* modelData = (ModelComponent*)modelPool.Get(GetEntityIndex(entity));
		if (modelData->myModelId == ModelID(-1))
			continue;
		ModelCommand.myModelId = modelData->myModelId;
		ModelCommand.myColor = { modelData->myColor.x,modelData->myColor.y,modelData->myColor.z,1 };
		ModelCommand.myUV = modelData->myUV;
		ModelCommand.myUVScale = modelData->myUVScale;
		ModelCommand.mySamplerState = modelData->mySamplerState;
		ModelCommand.myAdressMode = modelData->myAdressMode;
		ModelCommand.myEntityId = GetEntityIndex(entity);

		//oomfie... does a safety check in case it should be here at all
		//99.99% of the time its false so it should be fine... r-i-ight
		if (myCtx->myModelManager->GetModelType(modelData->myModelId) == ModelType::eTransparantModel)
		{
			auto* comp = scene.AssignComponent<CustomShaderComponent>(entity);
			comp->myShader = Shaders::eDefaultModelShader;
		}
		}

		//processses transform data
		{
		TransformComponent* transformData = (TransformComponent*)transformPool.Get(GetEntityIndex(entity));
		ModelCommand.myTransform = transformData->myCachedTransform; 
		}

		myRenderCommands.push_back(ModelCommand);
	}
}

void ISTE::ModelDrawerSystem::Draw()
{ 

	ID3D11DeviceContext* dCtx = myCtx->myDX11->GetContext();
	auto& modelList = myCtx->myModelManager->GetModelList(); 

	myCtx->myRenderStateManager->SetSampleState(AdressMode::eWrap, SamplerState::eBiliniear);
	BindShader();
	D3D11_MAPPED_SUBRESOURCE instanceBufferMappedData;
	D3D11_MAPPED_SUBRESOURCE gBufferMappedData;

	BatchedModelCommand*				mRC;
	Model*								curModel;
	std::vector<BatchedModelCommand>*	commandBatch;
	TextureID*							textureIDs; 
	int									meshID;
	size_t								size; 

	InstanceBuffer* instanceBufferData;
	CommonGBuffer* instanceGBuffer;
	ID3D11Buffer* buffers[2];

	size_t i;
	size_t batchSize;
	unsigned int offset[2] = { 0, 0 };
	unsigned int strides[2] = { sizeof(VertexData), sizeof(InstanceBuffer) };
	Mesh* curMesh;

	for (auto& element : myBatchedRenderCommands)
	{
		commandBatch	= &element.second.myBatchedCommands;
		curModel		= modelList.GetByIterator(element.second.myModelID);
		textureIDs		= element.second.myTextureID; 
		meshID			= element.second.myMeshID;
		size			= commandBatch->size();
		 
		i = 0;
		while (i < size)
		{
			dCtx->Map(myInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &instanceBufferMappedData);
			dCtx->Map(myCommonGBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &gBufferMappedData);

			instanceBufferData = (InstanceBuffer*)instanceBufferMappedData.pData;
			instanceGBuffer = (CommonGBuffer*)gBufferMappedData.pData;
			batchSize = 0;
			for (; batchSize < MODEL_INSTANCE_BATCH_SIZE && i < size; batchSize++)
			{
				mRC = &(*commandBatch)[i];
				instanceBufferData[batchSize].myTransform					= mRC->myTransform;
				instanceBufferData[batchSize].myUvScaleAndOffset.x			= mRC->myUVScale.x;
				instanceBufferData[batchSize].myUvScaleAndOffset.y			= mRC->myUVScale.y;
				instanceBufferData[batchSize].myUvScaleAndOffset.z			= mRC->myUVOffset.x;
				instanceBufferData[batchSize].myUvScaleAndOffset.w			= mRC->myUVOffset.y;
				instanceGBuffer->myInstanceData[batchSize].myIndex			= mRC->myEntityId;
				instanceGBuffer->myInstanceData[batchSize].myColor			= mRC->myColor;
				instanceGBuffer->myInstanceData[batchSize].myRenderFlags	= (float)mRC->myRenderFlags;
				Logger::LoggBatchedDrawCall(); 
				i++;
			}
			dCtx->Unmap(myInstanceBuffer.Get(), 0);
			dCtx->Unmap(myCommonGBuffer.Get(), 0); 

			myCtx->myTextureManager->PsBindTexture(textureIDs[ALBEDO_MAP], 0);
			myCtx->myTextureManager->PsBindTexture(textureIDs[NORMAL_MAP], 1);
			myCtx->myTextureManager->PsBindTexture(textureIDs[MATERIAL_MAP], 2);

			dCtx->PSSetConstantBuffers(2, 1, myCommonGBuffer.GetAddressOf()); 

			curMesh = &curModel->myMeshes[meshID];
			curMesh->BindIndexBuffer();

			buffers[0] = curMesh->GetVertexBuffer();
			buffers[1] = myInstanceBuffer.Get();

			dCtx->IASetVertexBuffers(0, 2, buffers, strides, offset);
			dCtx->DrawIndexedInstanced(curMesh->GetIndexCount(), batchSize, 0, 0, 0);
			Logger::LoggDrawCall(); 
		}
	} 
}


void ISTE::ModelDrawerSystem::DrawDepth(std::set<EntityID>& anEntity)
{ 
	std::unordered_map<uint64_t, ModelBatchValue> batchComms;
	
	Scene& scene = mySceneHandler->GetActiveScene();
	ModelManager* modelManager = myCtx->myModelManager;
	ComponentPool& modelPool = scene.GetComponentPool<ModelComponent>();
	ComponentPool& materialPool = scene.GetComponentPool<MaterialComponent>();
	ComponentPool& animPool = scene.GetComponentPool<AnimatorComponent>();
	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();

	BatchedModelCommand mC;
	ModelComponent* modelData;
	MaterialComponent* materialData;
	AnimatorComponent* animData;
	TransformComponent* transformData;
	ModelID modelID;
	TextureID texID[3];
	Model* model;
	size_t size;
	uint64_t key;
	ModelBatchValue MBvalue = {};

	for (auto entity : anEntity)
	{
		modelData = (ModelComponent*)modelPool.Get(GetEntityIndex(entity));
		animData = (AnimatorComponent*)animPool.Get(GetEntityIndex(entity));
		if (modelData == nullptr || animData != nullptr)
			continue;
		modelID = modelData->myModelId;
		{
			mC.myColor = { modelData->myColor.x,modelData->myColor.y,modelData->myColor.z,1 };
			mC.myUVOffset = modelData->myUV;
			mC.myUVScale = modelData->myUVScale;
			mC.myEntityId = GetEntityIndex(entity);
		}

		materialData = (MaterialComponent*)materialPool.Get(GetEntityIndex(entity));
		if (materialData == nullptr)
			continue;

		//processses transform data 
		transformData = (TransformComponent*)transformPool.Get(GetEntityIndex(entity));
		mC.myTransform = transformData->myCachedTransform; 

		if (modelID == ModelID(-1))
			continue;

		model = myCtx->myModelManager->GetModel(modelID);
		size = model->myMeshCount;


		for (size_t meshIndex = 0; meshIndex < size; meshIndex++)
		{
			memcpy(texID, materialData->myTextures[meshIndex], sizeof(TextureID) * MAX_MATERIAL_COUNT);

			unsigned int mod = (unsigned int)modelID << 16;
			unsigned int tex = (unsigned int)texID[0];
			unsigned int tex2 = (unsigned int)texID[1] << 16;
			unsigned int tex3 = (unsigned int)texID[2];

			unsigned int hash[3];
			hash[0] = mod | tex;
			hash[1] = tex2 | tex3;
			hash[2] = meshIndex;
			key = MurmurHash::MurmurHash2(hash, sizeof(int) * 3, 0);

			auto& value = batchComms.find(key);
			if (value == batchComms.end())
			{
				MBvalue.myModelID = modelID;
				MBvalue.myMeshID = meshIndex;
				 
				MBvalue.myTextureID[0] = texID[0];
				MBvalue.myTextureID[1] = texID[1];
				MBvalue.myTextureID[2] = texID[2];
				batchComms[key] = MBvalue; 
				batchComms[key].myBatchedCommands.reserve(MODEL_INSTANCE_BATCH_SIZE);
				batchComms[key].myBatchedCommands.emplace_back(mC);		//LKJBDAÖKJBDA
			}
			else
			{
				value->second.myBatchedCommands.emplace_back(mC);
			}
		}
	}




	ID3D11DeviceContext* dCtx = myCtx->myDX11->GetContext();
	auto& modelList = myCtx->myModelManager->GetModelList(); 

	myCtx->myDX11->GetContext()->VSSetShader(myVertexShader.Get(), 0, 0);
	myCtx->myDX11->GetContext()->PSSetShader(nullptr, 0, 0);
	myCtx->myDX11->GetContext()->IASetInputLayout(myInputLayout.Get());
	myCtx->myRenderStateManager->SetSampleState(AdressMode::eWrap, SamplerState::eBiliniear);

	D3D11_MAPPED_SUBRESOURCE instanceBufferMappedData;

	BatchedModelCommand	mRC;
	Model* curModel;
	std::vector<BatchedModelCommand>* commandBatch;
	const TextureID* textureIDs;
	int	meshID; 


	InstanceBuffer* instanceBufferData;

	size_t i;
	size_t batchSize;
	unsigned int offset[2] = { 0, 0 };
	unsigned int strides[2] = { sizeof(VertexData), sizeof(InstanceBuffer) };

	for (auto& element : batchComms)
	{
		commandBatch	= &element.second.myBatchedCommands;
		curModel		= modelList.GetByIterator(element.second.myModelID);
		textureIDs		= element.second.myTextureID;
		meshID			= element.second.myMeshID;
		size			= commandBatch->size();

		i = 0;
		while (i < size)
		{
			dCtx->Map(myInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &instanceBufferMappedData);

			instanceBufferData = (InstanceBuffer*)instanceBufferMappedData.pData;
			batchSize = 0;
			for (; batchSize < MODEL_INSTANCE_BATCH_SIZE && i < size; batchSize++)
			{
				mRC = (*commandBatch)[i];
				instanceBufferData[batchSize].myTransform = mRC.myTransform;
				Logger::LoggBatchedDrawCall();
				i++;
			}
			dCtx->Unmap(myInstanceBuffer.Get(), 0);

			dCtx->PSSetConstantBuffers(2, 1, myCommonGBuffer.GetAddressOf());

			Mesh& curMesh = curModel->myMeshes[meshID];
			curMesh.BindIndexBuffer();

			ID3D11Buffer* buffers[2] = { curMesh.GetVertexBuffer(), myInstanceBuffer.Get() };

			dCtx->IASetVertexBuffers(0, 2, buffers, strides, offset);
			dCtx->DrawIndexedInstanced(curMesh.GetIndexCount(), batchSize, 0, 0, 0);
			Logger::LoggDrawCall();
		}
	}

	size = batchComms.size();
	batchComms.clear();
}

void ISTE::ModelDrawerSystem::DrawDepth()
{

	ID3D11DeviceContext* dCtx = myCtx->myDX11->GetContext();
	auto& modelList = myCtx->myModelManager->GetModelList(); 

	myCtx->myDX11->GetContext()->VSSetShader(myVertexShader.Get(), 0, 0);
	myCtx->myDX11->GetContext()->PSSetShader(nullptr, 0, 0);
	myCtx->myDX11->GetContext()->IASetInputLayout(myInputLayout.Get());
	myCtx->myRenderStateManager->SetSampleState(AdressMode::eWrap, SamplerState::eBiliniear); 

	D3D11_MAPPED_SUBRESOURCE instanceBufferMappedData; 

	BatchedModelCommand*					mRC;
	Model*								curModel;
	std::vector<BatchedModelCommand>*	commandBatch;
	const TextureID*					textureIDs; 
	int									meshID;
	size_t								size;

	InstanceBuffer* instanceBufferData; 
	ID3D11Buffer* buffers[2];
	size_t i;
	size_t batchSize;
	unsigned int offset[2] = { 0, 0 };
	unsigned int strides[2] = { sizeof(VertexData), sizeof(InstanceBuffer) };
	Mesh* curMesh;
	for (auto& element : myBatchedRenderCommands)
	{
		commandBatch	= &element.second.myBatchedCommands;
		curModel		= modelList.GetByIterator(element.second.myModelID);
		textureIDs		= element.second.myTextureID; 
		meshID			= element.second.myMeshID;
		size			= commandBatch->size();
		 
		i = 0;
		while (i < size)
		{
			dCtx->Map(myInstanceBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &instanceBufferMappedData); 

			instanceBufferData = (InstanceBuffer*)instanceBufferMappedData.pData; 
			batchSize = 0;
			for (; batchSize < MODEL_INSTANCE_BATCH_SIZE && i < size; batchSize++)
			{
				mRC = &(*commandBatch)[i];
				instanceBufferData[batchSize].myTransform = mRC->myTransform; 
				Logger::LoggBatchedDrawCall(); 
				i++;
			}
			dCtx->Unmap(myInstanceBuffer.Get(), 0);  

			dCtx->PSSetConstantBuffers(2, 1, myCommonGBuffer.GetAddressOf()); 

			curMesh = &curModel->myMeshes[meshID];
			curMesh->BindIndexBuffer();

			buffers[0] = curMesh->GetVertexBuffer();
			buffers[1] = myInstanceBuffer.Get();

			dCtx->IASetVertexBuffers(0, 2, buffers, strides, offset);
			dCtx->DrawIndexedInstanced(curMesh->GetIndexCount(), batchSize, 0, 0, 0);
			Logger::LoggDrawCall(); 
		}
	} 
}

void ISTE::ModelDrawerSystem::ClearCommands()
{
	//size_t size = myBatchedRenderCommands.size(); 
	//myBatchedRenderCommands.clear(); 
	//myBatchedRenderCommands.reserve(size);

	for (auto& pair : myBatchedRenderCommands)
	{
		pair.second.myBatchedCommands.resize(0);
	}
}

void ISTE::ModelDrawerSystem::BindShader()
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->VSSetShader(myVertexShader.Get(), 0, 0);
	context->PSSetShader(myPixelShader.Get(), 0, 0);
	context->IASetInputLayout(myInputLayout.Get());
}

bool ISTE::ModelDrawerSystem::InitBuffers()
{
	HRESULT result; 
	//{ 
	//	D3D11_BUFFER_DESC objBufferDesc = {};
	//	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	//	objBufferDesc.ByteWidth = sizeof(VSObjectBuffer);
	//	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//	result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myVsObjBuffer.GetAddressOf());
	//	if (FAILED(result))
	//		return false;
	//}
	{
		D3D11_BUFFER_DESC objBufferDesc = {};
		objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		objBufferDesc.ByteWidth = sizeof(CommonGBuffer);
		objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myCommonGBuffer.GetAddressOf());
		if (FAILED(result))
			return false;
	}
	{
		D3D11_BUFFER_DESC bufferDesc = {};
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.ByteWidth = sizeof(InstanceBuffer) * MODEL_INSTANCE_BATCH_SIZE;
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		result = myCtx->myDX11->GetDevice()->CreateBuffer(&bufferDesc, NULL, myInstanceBuffer.GetAddressOf());
		if (FAILED(result))
			return false;
	}

	return true;
} 

void ISTE::ModelDrawerSystem::BindObjBuffer(const ModelCommand& aCommand)
{
	//ID3D11DeviceContext* context = myCtx->myDX11->GetContext();
	//
	//{
	//D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	//HRESULT res = context->Map(myVsObjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	//if (FAILED(res))
	//	return;
	//VSObjectBuffer* OBdata = (VSObjectBuffer*)(resource.pData);
	//
	//OBdata->myTransform = aCommand.myTransform;
	//context->Unmap(myVsObjBuffer.Get(), 0);
	//context->VSSetConstantBuffers(2, 1, myVsObjBuffer.GetAddressOf()); 
	//}
	//
	//{ 
	//D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	//HRESULT res = context->Map(myCommonGBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	//if (FAILED(res))
	//	return;
	//CommonGBuffer* OBdata = (CommonGBuffer*)(resource.pData);
	//
	//OBdata->myIndex = aCommand.myEntityId;
	//OBdata->myColor.x = aCommand.myColor.x;
	//OBdata->myColor.y = aCommand.myColor.y;
	//OBdata->myColor.z = aCommand.myColor.z;
	//
	//context->Unmap(myCommonGBuffer.Get(), 0);
	//context->PSSetConstantBuffers(2, 1, myCommonGBuffer.GetAddressOf());
	//}
}

bool ISTE::ModelDrawerSystem::InitShader(const std::string& aVSPath, const std::string& aPSPath)
{
	ID3D11Device* aDevice = myCtx->myDX11->GetDevice();
	HRESULT result;


	std::string vsData;
	DX11::CreateVertexShader(aVSPath, myVertexShader.GetAddressOf(), vsData);
	DX11::CreatePixelShader(aPSPath, myPixelShader.GetAddressOf());
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
			{
				"InstanceWorld",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				1,
				0,
				D3D11_INPUT_PER_INSTANCE_DATA,
				1
			},
			{
				"InstanceWorld",
				1,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				1,
				16,
				D3D11_INPUT_PER_INSTANCE_DATA,
				1
			},
			{
				"InstanceWorld",
				2,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				1,
				32,
				D3D11_INPUT_PER_INSTANCE_DATA,
				1
			},
			{
				"InstanceWorld",
				3,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				1,
				48,
				D3D11_INPUT_PER_INSTANCE_DATA,
				1
			},
			{
				"InstancedUVScaleAndOffset",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				1,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_INSTANCE_DATA,
				1
			}
		};

		int elementCount = sizeof(layout) / sizeof(layout[0]);
		result = aDevice->CreateInputLayout(layout, elementCount, vsData.data(), vsData.size(), myInputLayout.GetAddressOf());
		if (FAILED(result))
			return false;
	}
	return true;
}

