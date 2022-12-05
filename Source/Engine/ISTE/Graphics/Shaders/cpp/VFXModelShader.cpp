#include "VFXModelShader.h"
#include <ISTE/Context.h>
#include <ISTE/Graphics/DX11.h>
#include <ISTE/Graphics/Resources/TextureManager.h>
#include <ISTE/Graphics/Resources/ModelManager.h>

#include "ISTE/Graphics/ComponentAndSystem/ModelCommand.h" 

#include <ISTE/Graphics/ComponentAndSystem/TransformComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/ModelComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/TransperancyComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/CustomShaderComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/MaterialComponent.h>
#include <ISTE/Scene/SceneHandler.h>
#include <ISTE/Scene/Scene.h>

#include "ISTE/Graphics/RenderStateManager.h"

struct ObjectBuffer
{
	CU::Matrix4x4f myTransform;

	float myElaspedTime;
	CU::Vec3f garb;
};

bool ISTE::VFXModelShader::CreateBuffers()
{
	HRESULT result;
	//FrameBuffer init
	{
		//move out in to a shader class later
		D3D11_BUFFER_DESC objBufferDesc = {};
		objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		objBufferDesc.ByteWidth = sizeof(ObjectBuffer);
		objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myObjBuffer.GetAddressOf());
		if (FAILED(result))
			return false;
	}
	return true;
}

void ISTE::VFXModelShader::Render(EntityID aEntity)
{
	const int modelID = PrepEntity(aEntity);
	BindObjBuffer(aEntity);

	size_t meshSize = myCtx->myModelManager->GetModel(modelID)->myMeshCount;
	MaterialComponent* matComp = myCtx->mySceneHandler->GetActiveScene().GetComponent<MaterialComponent>(aEntity);
	for (size_t i = 0; i < meshSize; i++)
	{
		myCtx->myTextureManager->PsBindTexture(matComp->myTextures[i][ALBEDO_MAP], 0);
		myCtx->myTextureManager->PsBindTexture(matComp->myTextures[i][NORMAL_MAP], 1);
		myCtx->myTextureManager->PsBindTexture(matComp->myTextures[i][MATERIAL_MAP], 2);
		
		myCtx->myModelManager->BindMesh(modelID, i);
		myCtx->myModelManager->RenderMesh(modelID, i);
	}
} 

const int ISTE::VFXModelShader::PrepEntity(EntityID aEntity)
{
	ModelComponent* modelData = myCtx->mySceneHandler->GetActiveScene().GetComponent<ModelComponent>(aEntity);
	return modelData->myModelId;
}

void ISTE::VFXModelShader::BindObjBuffer(const EntityID aEntity)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	HRESULT res = context->Map(myObjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	ObjectBuffer* OBdata = (ObjectBuffer*)(resource.pData);

	TransformComponent* transformData = myCtx->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(aEntity);
	OBdata->myTransform = transformData->myCachedTransform;

	CustomShaderComponent* shaderData = myCtx->mySceneHandler->GetActiveScene().GetComponent<CustomShaderComponent>(aEntity);
	OBdata->myElaspedTime = shaderData->myElaspedTime;
	 



	context->Unmap(myObjBuffer.Get(), 0);
	context->VSSetConstantBuffers(2, 1, myObjBuffer.GetAddressOf());
	context->PSSetConstantBuffers(2, 1, myObjBuffer.GetAddressOf());
}
