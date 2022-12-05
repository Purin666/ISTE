#include "DecalDrawerSystem.h"
#include <d3d11.h>


#include "ISTE/Context.h"
#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/ECSB/ComponentPool.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"

#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/Resources/ShaderManager.h"
#include "ISTE/Graphics/RenderStateManager.h"
#include "ISTE/Graphics/DX11.h"
#include "ISTE/Graphics/GraphicsEngine.h"

#include "ISTE/Graphics/ComponentAndSystem/MaterialComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/DecalComponent.h"


struct VertexShaderData
{
	CU::Matrix4x4f myModelToWorld;  
};

struct DecalBufferData
{  
	CU::Matrix4x4f myModelToWorldInv;
	CU::Matrix4x4f myModelToWorld;

	float myDecalFlag;
	float myLerpValues[3];

	float EntityIndex;
	float Threshold;
	float garb[2];
};


bool ISTE::DecalDrawerSystem::Init()
{
	myCtx = Context::Get();
	myCubeID = myCtx->myModelManager->LoadUnitCube();
	if (!InitShader("Shaders/Default_VS.cso","Shaders/Decal_PS.cso"))
		return false;
	if (!InitDecalObjBuffer())
		return false;

	return true;
}

void ISTE::DecalDrawerSystem::PrepareRenderCommands()
{
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ComponentPool&		decalPool		= scene.GetComponentPool<DecalComponent>();
	ComponentPool&		transformPool	= scene.GetComponentPool<TransformComponent>();
	ComponentPool&		materialPool	= scene.GetComponentPool<MaterialComponent>();
	Camera				mainCamera		= myCtx->myGraphicsEngine->GetCamera();
	DecalRenderCommand	dRC; 
	TransformComponent* traComp;
	MaterialComponent*	matComp;
	DecalComponent*		decComp;

	for (auto& ent : myEntities[0])
	{
		dRC.myEntIndex = GetEntityIndex(ent);
		
		traComp = (TransformComponent*)transformPool.Get(dRC.myEntIndex);
		dRC.myTransform		= traComp->myCachedTransform;
		dRC.myTransformInv	= CU::Matrix4x4f::Inverse(traComp->myCachedTransform);
		dRC.myDiffLengthToCamera = (mainCamera.GetTransform().GetTranslationV3() - dRC.myTransform.GetTranslationV3()).LengthSqr();

		matComp = (MaterialComponent*)materialPool.Get(dRC.myEntIndex);
		dRC.myTextureIDs[0]	= matComp->myTextures[0][0];
		dRC.myTextureIDs[1]	= matComp->myTextures[0][1];
		dRC.myTextureIDs[2] = matComp->myTextures[0][2]; 

		decComp = (DecalComponent*)decalPool.Get(dRC.myEntIndex);
		memcpy(dRC.myLerpValues, &decComp->myLerpValues.x, sizeof(CU::Vec3f));
		dRC.myCoverageFlag	= decComp->myCoverageFlag;
		dRC.myThreshold		= decComp->myAngleThreshold * ISTE::DegToRad;
		
		myDecalRenderCommands.Enqueue(dRC);
	}
}

void ISTE::DecalDrawerSystem::AddDecalRenderCommand(const DecalRenderCommand& aCommand)
{
	myDecalRenderCommands.Enqueue(aCommand);
}

void ISTE::DecalDrawerSystem::Draw()
{
	DecalRenderCommand com;
	size_t i = 0;
	size_t size = myDecalRenderCommands.GetSize();
	if (size == 0)
		return;

	myCtx->myDX11->GetIntermediateGBuffer().SetAsActiveTarget(myCtx->myDX11->GetDepthBuffer()->GetDepthStencilRTV());
	myCtx->myDX11->GetIntermediateGBuffer().ClearTextures();
	myCtx->myDX11->GetMainGBuffer().PSSetAllResources(4, false);
	myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthLess, ReadWriteState::eRead);
	myCtx->myShaderManager->GetFullscreenEffect(eFullscreenEffects::ePPGbufferCopy).Render();

	myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthGreaterOrEqual, ReadWriteState::eRead);
	myCtx->myRenderStateManager->SetRasterState(RasterizerState::eFrontFaceCulling);
	myCtx->myRenderStateManager->SetBlendState(BlendState::eAlphaBlend);

	BindShader();
	myCtx->myModelManager->BindMesh(myCubeID, 0);
	for (; i < size; i++)
	{
		com = myDecalRenderCommands.Dequeue();
		BindObjBuffer(&com);
		 
		myCtx->myTextureManager->PsBindTexture(com.myTextureIDs[ALBEDO_MAP],   0);
		myCtx->myTextureManager->PsBindTexture(com.myTextureIDs[NORMAL_MAP],   1);
		myCtx->myTextureManager->PsBindTexture(com.myTextureIDs[MATERIAL_MAP], 2);

		myCtx->myModelManager->RenderMesh(myCubeID, 0);
	}

	myCtx->myRenderStateManager->SetBlendState(BlendState::eDisabled);
	myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthLess);
	myCtx->myRenderStateManager->SetRasterState(RasterizerState::eBackFaceCulling);
	ID3D11ShaderResourceView* nullsrv[7] = { };
	myCtx->myDX11->GetContext()->PSSetShaderResources(4,7, nullsrv);
	myCtx->myDX11->GetMainGBuffer().SetAsActiveTarget();
	myCtx->myDX11->GetIntermediateGBuffer().PSSetAllResources(4, false); 
	myCtx->myShaderManager->GetFullscreenEffect(eFullscreenEffects::ePPGbufferCopy).Render();

}

void ISTE::DecalDrawerSystem::DrawDebugLines()
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();
	context->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP);
	context->VSSetShader(myDecalVertexShader.Get(), 0, 0);
	context->PSSetShader(myColorPixelShader.Get(), 0, 0);
	context->IASetInputLayout(myInputLayout.Get()); 
	myCtx->myModelManager->BindMesh(myCubeID, 0);

	DecalRenderCommand com; 
	D3D11_MAPPED_SUBRESOURCE data; 
	VertexShaderData* vsBufferData;
	size_t i = 0;
	size_t size = myDecalRenderCommands.GetSize(); 
	for (; i < size; i++)
	{
		com = myDecalRenderCommands.Dequeue();
		
		context->Map(myDecalVSObjBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &data);
		vsBufferData = (VertexShaderData*)data.pData;
		vsBufferData->myModelToWorld = com.myTransform;
		context->Unmap(myDecalVSObjBuffer.Get(), 0); 
		context->VSSetConstantBuffers(2, 1, myDecalVSObjBuffer.GetAddressOf());

		myCtx->myModelManager->RenderMesh(myCubeID, 0);
	}
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void ISTE::DecalDrawerSystem::ClearRenderCommands()
{
	size_t size = myDecalRenderCommands.GetSize();
	myDecalRenderCommands.Clear();
	myDecalRenderCommands.Reserve(size);
}

bool ISTE::DecalDrawerSystem::InitShader(std::string aVSPath, std::string aPSPath)
{
	ID3D11Device* aDevice = myCtx->myDX11->GetDevice();
	HRESULT result;
	std::string vsData;
	DX11::CreateVertexShader(aVSPath, myDecalVertexShader.GetAddressOf(), vsData);
	DX11::CreatePixelShader(aPSPath, myDecalPixelShader.GetAddressOf());
	DX11::CreatePixelShader("Shaders/Color_PS.cso", myColorPixelShader.GetAddressOf());
	
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
		result = aDevice->CreateInputLayout(layout, elementCount, vsData.data(), vsData.size(), myInputLayout.GetAddressOf());
		if (FAILED(result))
			return false;
	}
	return true;
}

bool ISTE::DecalDrawerSystem::InitDecalObjBuffer()
{
	HRESULT result;

	D3D11_BUFFER_DESC objBufferDesc = {};
	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objBufferDesc.ByteWidth = sizeof(DecalBufferData);
	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myDecalPSObjBuffer.GetAddressOf());
	if (FAILED(result))
		return false;

	objBufferDesc = {};
	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objBufferDesc.ByteWidth = sizeof(VertexShaderData);
	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myDecalVSObjBuffer.GetAddressOf());
	if (FAILED(result))
		return false;

	return true;
}

void ISTE::DecalDrawerSystem::BindShader()
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->VSSetShader(myDecalVertexShader.Get(), 0, 0);
	context->PSSetShader(myDecalPixelShader.Get(), 0, 0);
	context->IASetInputLayout(myInputLayout.Get());
}

void ISTE::DecalDrawerSystem::BindObjBuffer(const DecalRenderCommand* aCom)
{
	ID3D11DeviceContext* dCtx = myCtx->myDX11->GetContext();
	D3D11_MAPPED_SUBRESOURCE data;

	HRESULT res = dCtx->Map(myDecalPSObjBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &data);
	if (FAILED(res))
		return;

	DecalBufferData* psBufferData = (DecalBufferData*)data.pData;
	psBufferData->myDecalFlag			= (float)aCom->myCoverageFlag;
	psBufferData->Threshold				= aCom->myThreshold;
	psBufferData->EntityIndex			= aCom->myEntIndex;
	psBufferData->myModelToWorldInv		= aCom->myTransformInv;
	psBufferData->myModelToWorld		= aCom->myTransform;
	memcpy(psBufferData->myLerpValues, aCom->myLerpValues, sizeof(float) * 3);
	dCtx->Unmap(myDecalPSObjBuffer.Get(), 0);
	

	data = {};
	res = dCtx->Map(myDecalVSObjBuffer.Get(), 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &data);
	if (FAILED(res))
		return;
	VertexShaderData* vsBufferData = (VertexShaderData*)data.pData;
	vsBufferData->myModelToWorld	= aCom->myTransform;
	dCtx->Unmap(myDecalVSObjBuffer.Get(), 0);
	
	
	dCtx->VSSetConstantBuffers(2, 1, myDecalVSObjBuffer.GetAddressOf());
	dCtx->PSSetConstantBuffers(2, 1, myDecalPSObjBuffer.GetAddressOf());
}
