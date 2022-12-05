#include "ModelManager.h"

#include <string>
#include <fstream>
#include <codecvt>
#include <d3d11.h>
#include <unordered_map>

#include <assimp/Importer.hpp>
#include <Assimp/scene.h>
#include <Assimp/postprocess.h>
#include <assimp/material.h>

#include <ISTE/Math/Vec.h>
#include <ISTE/Logger/Logger.h>
#include "ISTE/Graphics/GraphicsEngine.h"
#include <ISTE/Graphics/Resources/TextureManager.h>
#include <ISTE/Graphics/DX11.h>
#include <ISTE/Context.h>

#include <ISTE/Scene/SceneHandler.h>
#include <ISTE/Scene/Scene.h>
#include <ISTE/Graphics/ComponentAndSystem/ModelComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h>

#include <ISTE/Graphics/Resources/Mesh.h>

struct PreviewObjectBuffer
{
	CU::Matrix4x4f myTransform;
};


ISTE::ModelManager::ModelManager()
{ 
	myCtx = nullptr;
	mySceneHandler = nullptr;
}

ISTE::ModelManager::~ModelManager()
{
	myModels.ClearFromHeap();  
}

bool ISTE::ModelManager::Init()
{
	myCtx = Context::Get(); 
	myFbxLoader.Init();
	mySceneHandler = myCtx->mySceneHandler;
	if (!InitShaders("Shaders/Default_VS.cso","Shaders/Preview_PS.cso"))
		return false;
	myPreviewCamera.SetPerspectiveProjection(90, { 64,64 }, 0.1f, 200);
	return true;
}

ISTE::ModelManager::ModelLoadResult ISTE::ModelManager::LoadModel(std::string aModelPath)
{ 
    if (!myModels.Exists(aModelPath))
        return LoadNewModel(aModelPath.c_str());
    else
		return {true, myModels.Get(aModelPath)->myIndex};
}

ISTE::ModelManager::ModelLoadResult ISTE::ModelManager::LoadModel(EntityID aEntityID, std::string aModelPath)
{ 
	if(!IsEntityIndexValid(aEntityID))
		return{ false, ModelID(-1) };
	//Loads the model
	ModelLoadResult loadResult = LoadModel(aModelPath); 

	//in case it failed loading		actually should it?
	//if (!loadResult)
	//	return{ false, -1 };

	//checks if it has a model component; assigns it if not; and sets modelId to the component
	ModelComponent* modelComp = mySceneHandler->GetActiveScene().GetComponent<ModelComponent>(aEntityID);
	if (modelComp == nullptr)
		modelComp = mySceneHandler->GetActiveScene().AssignComponent<ModelComponent>(aEntityID);
	
	modelComp->myModelId = loadResult;
	return loadResult;
}

int ISTE::ModelManager::LoadUnitCube()
{
	std::string key = "UnitCube";
	if (myModels.Exists(key))
		return myModels.Get(key)->myIndex;

	std::vector<VertexData> vertecies = {
		{CU::Vec3f(0.5f, -0.5f, 0.5f),		CU::Vec4f(1, 1, 1, 1.f),	CU::Vec2f(0, 1),	CU::Vec3f(0.0f, 0.0f, 1.f),		CU::Vec3f(-1.f, 0.0f, 0.f),	CU::Vec3f(0.f, -1.f, 0.f)},
		{CU::Vec3f(0.5f, 0.5f, 0.5f),		CU::Vec4f(1, 1, 1, 1.f),	CU::Vec2f(0, 0),	CU::Vec3f(0.0f, 0.0f, 1.f),		CU::Vec3f(-1.f, 0.0f, 0.f),	CU::Vec3f(0.f, -1.f, 0.f)},
		{CU::Vec3f(-0.5f, 0.5f, 0.5f),		CU::Vec4f(1, 1, 1, 1.f),	CU::Vec2f(1, 0),	CU::Vec3f(0.0f, 0.0f, 1.f),		CU::Vec3f(-1.f, 0.0f, 0.f),	CU::Vec3f(0.f, -1.f, 0.f)},
		{CU::Vec3f(-0.5f, -0.5f, 0.5f),		CU::Vec4f(1, 1, 1, 1.f),	CU::Vec2f(1, 1),	CU::Vec3f(0.0f, 0.0f, 1.f),		CU::Vec3f(-1.f, 0.0f, 0.f),	CU::Vec3f(0.f, -1.f, 0.f)},

		{CU::Vec3f(-0.5f, -0.5f, 0.5f),		CU::Vec4f(1, 0, 0, 1.f),	CU::Vec2f(0, 1),	CU::Vec3f(-1.0f, 0.0f, 0.f),	CU::Vec3f(0, 0.0f, 1.f),	CU::Vec3f(0.f, -1.f, 0.f)},
		{CU::Vec3f(-0.5f, 0.5f, 0.5f),		CU::Vec4f(1, 0, 0, 1.f),	CU::Vec2f(0, 0),	CU::Vec3f(-1.0f, 0.0f, 0.f),	CU::Vec3f(0, 0.0f, 1.f),	CU::Vec3f(0.f, -1.f, 0.f)},
		{CU::Vec3f(-0.5f, 0.5f, -0.5f),		CU::Vec4f(1, 0, 0, 1.f),	CU::Vec2f(1, 0),	CU::Vec3f(-1.0f, 0.0f, 0.f),	CU::Vec3f(0, 0.0f, 1.f),	CU::Vec3f(0.f, -1.f, 0.f)},
		{CU::Vec3f(-0.5f, -0.5f, -0.5f),	CU::Vec4f(1, 0, 0, 1.f),	CU::Vec2f(1, 1),	CU::Vec3f(-1.0f, 0.0f, 0.f),	CU::Vec3f(0, 0.0f, 1.f),	CU::Vec3f(0.f, -1.f, 0.f)},

		{CU::Vec3f(-0.5f, -0.5f, -0.5f),	CU::Vec4f(0, 1, 0, 1.f),	CU::Vec2f(0, 1),	CU::Vec3f(0.0f, 0.0f, -1.f), 	CU::Vec3f(1.f, 0.0f, 0.f),	CU::Vec3f(0.f, -1.f, 0.f)},
		{CU::Vec3f(-0.5f, 0.5f, -0.5f),		CU::Vec4f(0, 1, 0, 1.f),	CU::Vec2f(0, 0),	CU::Vec3f(0.0f, 0.0f, -1.f), 	CU::Vec3f(1.f, 0.0f, 0.f),	CU::Vec3f(0.f, -1.f, 0.f)},
		{CU::Vec3f(0.5f, 0.5f, -0.5f),		CU::Vec4f(0, 1, 0, 1.f),	CU::Vec2f(1, 0),	CU::Vec3f(0.0f, 0.0f, -1.f), 	CU::Vec3f(1.f, 0.0f, 0.f),	CU::Vec3f(0.f, -1.f, 0.f)},
		{CU::Vec3f(0.5f, -0.5f, -0.5f),		CU::Vec4f(0, 1, 0, 1.f),	CU::Vec2f(1, 1),	CU::Vec3f(0.0f, 0.0f, -1.f), 	CU::Vec3f(1.f, 0.0f, 0.f),	CU::Vec3f(0.f, -1.f, 0.f)},

		{CU::Vec3f(0.5f, -0.5f, -0.5f),		CU::Vec4f(0, 0, 1,1.f),		CU::Vec2f(0, 1),	CU::Vec3f(1.f, 0.0f, 0.f),  	CU::Vec3f(0, 0.0f, -1.f),	CU::Vec3f(0.f, -1.f, 0.f)},
		{CU::Vec3f(0.5f, 0.5f, -0.5f),		CU::Vec4f(0, 0, 1,1.f),		CU::Vec2f(0, 0),	CU::Vec3f(1.f, 0.0f, 0.f),  	CU::Vec3f(0, 0.0f, -1.f),	CU::Vec3f(0.f, -1.f, 0.f)},
		{CU::Vec3f(0.5f, 0.5f, 0.5f),		CU::Vec4f(0, 0, 1,1.f),		CU::Vec2f(1, 0),	CU::Vec3f(1.f, 0.0f, 0.f),  	CU::Vec3f(0, 0.0f, -1.f),	CU::Vec3f(0.f, -1.f, 0.f)},
		{CU::Vec3f(0.5f, -0.5f, 0.5f),		CU::Vec4f(0, 0, 1,1.f),		CU::Vec2f(1, 1),	CU::Vec3f(1.f, 0.0f, 0.f),  	CU::Vec3f(0, 0.0f, -1.f),	CU::Vec3f(0.f, -1.f, 0.f)},

		{CU::Vec3f(0.5f, 0.5f, 0.5f),		CU::Vec4f(1, 1, 0,1.f),		CU::Vec2f(0, 1),	CU::Vec3f(0.f, 1.0f, 0.f),		CU::Vec3f(1.f, 0.0f, 0.f),	CU::Vec3f(0.f, 0.f, -1.f)},
		{CU::Vec3f(0.5f, 0.5f, -0.5f),		CU::Vec4f(1, 1, 0,1.f),		CU::Vec2f(0, 0),	CU::Vec3f(0.f, 1.0f, 0.f),		CU::Vec3f(1.f, 0.0f, 0.f),	CU::Vec3f(0.f, 0.f, -1.f)},
		{CU::Vec3f(-0.5f, 0.5f, -0.5f),		CU::Vec4f(1, 1, 0,1.f),		CU::Vec2f(1, 0),	CU::Vec3f(0.f, 1.0f, 0.f),		CU::Vec3f(1.f, 0.0f, 0.f),	CU::Vec3f(0.f, 0.f, -1.f)},
		{CU::Vec3f(-0.5f, 0.5f, 0.5f),		CU::Vec4f(1, 1, 0,1.f),		CU::Vec2f(1, 1),	CU::Vec3f(0.f, 1.0f, 0.f),		CU::Vec3f(1.f, 0.0f, 0.f),	CU::Vec3f(0.f, 0.f, -1.f)},
											
		{CU::Vec3f(-0.5f, -0.5f, 0.5f),		CU::Vec4f(1, 0, 1,1.f),		CU::Vec2f(0, 1),	CU::Vec3f(0.f, -1.0f, 0.f),		CU::Vec3f(1.f, 0.0f, 0.f),	CU::Vec3f(0.f, 0.f, 1.f)},
		{CU::Vec3f(-0.5f, -0.5f, -0.5f),	CU::Vec4f(1, 0, 1,1.f),		CU::Vec2f(0, 0),	CU::Vec3f(0.f, -1.0f, 0.f),		CU::Vec3f(1.f, 0.0f, 0.f),	CU::Vec3f(0.f, 0.f, 1.f)},
		{CU::Vec3f(0.5f, -0.5f, -0.5f),		CU::Vec4f(1, 0, 1,1.f),		CU::Vec2f(1, 0),	CU::Vec3f(0.f, -1.0f, 0.f),		CU::Vec3f(1.f, 0.0f, 0.f),	CU::Vec3f(0.f, 0.f, 1.f)},
		{CU::Vec3f(0.5f, -0.5f, 0.5f),		CU::Vec4f(1, 0, 1,1.f),		CU::Vec2f(1, 1),	CU::Vec3f(0.f, -1.0f, 0.f),		CU::Vec3f(1.f, 0.0f, 0.f),	CU::Vec3f(0.f, 0.f, 1.f)},
	};
	std::vector<unsigned int> indecies = {
		0,  1,  2,
		0,  2,  3,
		4,  5,  6,
		4,  6,  7,
		8,  9,  10,
		8,  10, 11,
		12, 13, 14,
		12, 14, 15,
		16, 17, 18,
		16, 18, 19,
		20, 21, 22,
		20, 22, 23
	}; 
	
	Model* createdModel = new Model(); 
	createdModel->myMeshes = new Mesh[1];
	Mesh& cubeMesh = createdModel->myMeshes[0];
	createdModel->myMeshCount = 1;
	cubeMesh.SetVertexList(vertecies);
	cubeMesh.SetIndexList(indecies); 
	if (!cubeMesh.Init())
		return -1;  

	createdModel->myTextures[0][ALBEDO_MAP] = Context::Get()->myTextureManager->LoadTexture(L"../Assets/Sprites/Rock_c.png", true);
	createdModel->myTextures[0][NORMAL_MAP] = Context::Get()->myTextureManager->LoadTexture(L"../Assets/Sprites/Rock_n.png", false);
	createdModel->myTextures[0][MATERIAL_MAP] = Context::Get()->myTextureManager->LoadTexture(L"../Assets/Sprites/Rock_m.png", false);

	createdModel->myAABB.myMax = { 1,1,1 };

	int index = myModels.Insert(key, createdModel);
	myModels.GetByIterator(index)->myIndex = index;
	return index;
}

int ISTE::ModelManager::LoadPlane(int aSubDivAmount)
{
	//checks if model has already been created
	std::string key = "Plane_" + std::to_string(aSubDivAmount);
	if (myModels.Exists(key))
		return myModels.Get(key)->myIndex;
	
	std::vector<VertexData> vertecies;
	vertecies.reserve((size_t)((2 + aSubDivAmount) * (2 + aSubDivAmount)));
	std::vector<unsigned int> indecies;
	indecies.reserve(6 * (pow(aSubDivAmount + 1, 2)));

	float subdiv = (float)aSubDivAmount;

	//vertex
	for (int i = 0; i < 2 + subdiv; i++)
	{
		for (int j = 0; j < 2 + subdiv; j++)
		{
			VertexData vertex = {};

			vertex.myPosition.x = (j / (2 + subdiv)) - 0.5f;
			vertex.myPosition.z = (i / (2 + subdiv)) - 0.5f;

			vertex.myTexCoord.x = j / (2 + subdiv);
			vertex.myTexCoord.y = i / (2 + subdiv);

			//assumed to always point uppwoards
			vertex.myNormal.y = 1;
			vertex.myTangent  = vertex.myNormal.Cross({ 0.f,0.f,1.f }).GetNormalized();
			vertex.myBiNormal = vertex.myNormal.Cross({ 1.f,0.f,0.f }).GetNormalized();

			//default material color
			vertex.myColor.x = 0.5f;
			vertex.myColor.y = 0.5f;
			vertex.myColor.z = 0.5f;

			vertecies.emplace_back(vertex);
		}
	}

	//indecies
	for (int i = 0; i < vertecies.size(); i++)
	{
		if ((i - (2 + subdiv) < 0) ||
			((i + 1) % (2 + (int)subdiv) == 0 && i + 1 != 2 + subdiv))
			continue;

		indecies.push_back(i);
		indecies.push_back(i - (2 + subdiv) + 1);
		indecies.push_back(i - (2 + subdiv));

		indecies.push_back(i);
		indecies.push_back(i + 1);
		indecies.push_back(i - (2 + subdiv) + 1);
	}
	Model* createdModel = new Model();
	createdModel->myMeshes = new Mesh[1];
	Mesh& createdMesh = createdModel->myMeshes[0];
	createdMesh.SetVertexList(vertecies);
	createdMesh.SetIndexList(indecies);  
	createdModel->myMeshCount = 1;
	if (!createdMesh.Init())
		return -1;
	 

	ModelID index = myModels.Insert(key, createdModel);
	myModels.GetByIterator(index)->myIndex = index;
	return index;
}

void ISTE::ModelManager::OverwriteTexture(ModelID aModelIt, TextureID aTextureIt, int aTextureType)
{
	Model* model = myModels.GetByIterator(aModelIt); 
	size_t size = model->myMeshCount;
	for(size_t i = 0; i < size; i++)
		model->myTextures[i][aTextureType] = aTextureIt; 
}

int ISTE::ModelManager::GetMeshCount(ModelID aModelId)
{
	return myModels.GetByIterator(aModelId)->myMeshCount;
}

ISTE::ModelType ISTE::ModelManager::GetModelType(ModelID aModelId)
{
	return myModels.GetByIterator(aModelId)->myModelType;
}

std::unordered_map<std::string, int>& ISTE::ModelManager::GetBoneNameToIdMap(ModelID aModelId)
{
	return myModels.GetByIterator(aModelId)->myBoneNameToId;
}

void ISTE::ModelManager::BindMesh(ModelID aModelId, int meshIndex)
{ 
	myModels.GetByIterator(aModelId)->myMeshes[meshIndex].BindMesh();
	//myCtx->myTextureManager->PsBindTexture(myModel->myTextures[meshIndex][AlbedoMap], 0);
	//myCtx->myTextureManager->PsBindTexture(myModel->myTextures[meshIndex][NormalMap], 1);
	//myCtx->myTextureManager->PsBindTexture(myModel->myTextures[meshIndex][MaterialMap], 2); 
}
 

void ISTE::ModelManager::RenderMesh(ModelID aModelId, int meshIndex)
{
	myCtx->myDX11->GetContext()->DrawIndexed(myModels.GetByIterator(aModelId)->myMeshes[meshIndex].GetIndexCount(), 0, 0);

	Logger::LoggDrawCall();
}

void ISTE::ModelManager::RenderModel(ModelID aModelId)
{
	size_t meshCount = myModels.GetByIterator(aModelId)->myMeshCount;
	Mesh* meshList = myModels.GetByIterator(aModelId)->myMeshes;
	Model& curModel = *myModels.GetByIterator(aModelId);

	//im mad 
	//myCtx->myTextureManager->PsBindTexture(curModel.myTextures[AlbedoMap], 0);
	//myCtx->myTextureManager->PsBindTexture(curModel.myTextures[NormalMap], 1);
	//myCtx->myTextureManager->PsBindTexture(curModel.myTextures[MaterialMap], 2);

	for (size_t i = 0; i < meshCount; i++)
	{
		meshList[i].BindMesh();

		myCtx->myDX11->GetContext()->DrawIndexed(meshList[i].GetIndexCount(), 0, 0);
		Logger::LoggDrawCall();
	}

} 

ISTE::ModelManager::ModelLoadResult ISTE::ModelManager::LoadNewModel(std::string aFilePath)
{
	//loads model and caches it
	FBXLoader::FBXResult res = myFbxLoader.LoadModel(aFilePath);
	Model* model = res.myModel; 
	if (res.mySuccesFlag == false)
	{
		return { false, LoadModel("../Assets/Models/ERROR.fbx") };
	}
	ModelID index = myModels.Insert(aFilePath, model);
	myModels.GetByIterator(index)->myIndex = index;
	

	//----render the preview image
	//gets the furthest axis in the aabb 
	//this is used to position the model in the preview image
	//float furthestDist = max(model->myAABB.myMax.x, max(model->myAABB.myMax.y, model->myAABB.myMax.z)); 
	//Camera* PrevCam = &myCtx->myGraphicsEngine->GetCamera();
	//myCtx->myGraphicsEngine->SetCamera(myPreviewCamera);
	//myCtx->myGraphicsEngine->BindFrameBuffer();
	//BindShaders();
	//CU::Matrix4x4f tf;  
	//tf(4, 3) = furthestDist * 1.5f;
	//BindObjBuffer(tf);
	//myModelPreviews[index].Create(64, 64, DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM_SRGB);
	//myModelPreviews[index].SetActiveTarget();
	//RenderModel(index);

	//resets to prev rendering settings
	//myCtx->myDX11->GetBackBuffer()->SetActiveTarget();
	//myCtx->myGraphicsEngine->SetCamera(*PrevCam);
	
	return { res.mySuccesFlag, index };
}

bool ISTE::ModelManager::InitShaders(const char* aVsPath, const char* aPsPath)
{
	std::string vsData;
	if (!DX11::CreateVertexShader(aVsPath, myPreviewVS.GetAddressOf(), vsData))
		return false;
	if (!DX11::CreatePixelShader(aPsPath, myPreviewPS.GetAddressOf()))
		return false;

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
	HRESULT result = myCtx->myDX11->GetDevice()->CreateInputLayout(layout, elementCount, vsData.data(), vsData.size(), myPreviewLayout.GetAddressOf());
	if (FAILED(result))
		return false;  
	 
	{
	D3D11_BUFFER_DESC objBufferDesc = {};
	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objBufferDesc.ByteWidth = sizeof(PreviewObjectBuffer);
	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myPreviewObjBuffer.GetAddressOf());
	if (FAILED(result))
		return false;
	}

	if (FAILED(result))
		return false;
	return true;
}

void ISTE::ModelManager::BindShaders() 
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	context->VSSetShader(myPreviewVS.Get(), 0, 0);
	context->PSSetShader(myPreviewPS.Get(), 0, 0);
	context->IASetInputLayout(myPreviewLayout.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ISTE::ModelManager::BindObjBuffer(CU::Matrix4x4f aTransform)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	HRESULT res = context->Map(myPreviewObjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	PreviewObjectBuffer* OBdata = (PreviewObjectBuffer*)(resource.pData);

	OBdata->myTransform = aTransform;
	context->Unmap(myPreviewObjBuffer.Get(), 0);
	context->VSSetConstantBuffers(2, 1, myPreviewObjBuffer.GetAddressOf());
}
 

int ISTE::ModelManager::LoadSpriteQuad()
{ 
	std::string key = "SpriteQuad"; 
	if (myModels.Exists(key))
		return myModels.Get(key)->myIndex;

	std::vector<VertexData> vertecies;
	vertecies.reserve(4);
	std::vector<unsigned int> indecies;
	indecies.reserve(6);

	VertexData vertex;
	vertex.myColor = { 1,1,1,1 };
	vertex.myNormal.z = -1;
	vertex.myTangent = vertex.myNormal.Cross({ 0.f,0.f,1.f }).GetNormalized();
	vertex.myBiNormal = vertex.myNormal.Cross({ 1.f,0.f,0.f }).GetNormalized();

	vertex.myPosition = { -0.5, -0.5, 0 };
	vertex.myTexCoord = { 0, 1}; 
	vertecies.push_back(vertex);
	vertex.myPosition = { -0.5, 0.5, 0 };
	vertex.myTexCoord = { 0, 0 };
	vertecies.push_back(vertex);
	vertex.myPosition = { 0.5, 0.5, 0 };
	vertex.myTexCoord = { 1, 0 };
	vertecies.push_back(vertex);
	vertex.myPosition = { 0.5, -0.5, 0 };
	vertex.myTexCoord = { 1, 1 };
	vertecies.push_back(vertex);

	indecies.push_back(0);
	indecies.push_back(1);
	indecies.push_back(2);

	indecies.push_back(2);
	indecies.push_back(3);
	indecies.push_back(0);


	Model* createdModel = new Model();
	createdModel->myMeshes = new Mesh[1];
	Mesh& createdMesh = createdModel->myMeshes[0];
	createdModel->myMeshCount = 1;
	createdMesh.SetVertexList(vertecies);
	createdMesh.SetIndexList(indecies);
	if (!createdMesh.Init())
		return -1;
	 			
	createdModel->myTextures[0][ALBEDO_MAP] = myCtx->myTextureManager->LoadTexture(L"../Assets/Sprites/ERROR.dds",true);
	createdModel->myTextures[0][NORMAL_MAP] = myCtx->myTextureManager->LoadTexture(L"../Assets/Sprites/T_Default_N.dds");
	createdModel->myTextures[0][MATERIAL_MAP] = myCtx->myTextureManager->LoadTexture(L"../Assets/Sprites/T_Default_M.dds");

	ModelID index = myModels.Insert(key, createdModel);
	myModels.GetByIterator(index)->myIndex = index;
	return index;
}