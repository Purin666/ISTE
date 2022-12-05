#include "ModelShader.h"
#include <ISTE/Context.h>
#include <ISTE/Graphics/DX11.h>
#include <ISTE/Graphics/Resources/TextureManager.h>
#include <ISTE/Graphics/Resources/ModelManager.h>

#include "ISTE/Graphics/ComponentAndSystem/ModelCommand.h" 

#include <ISTE/Graphics/ComponentAndSystem/TransformComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/ModelComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/MaterialComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/TransperancyComponent.h>
#include <ISTE/Scene/SceneHandler.h>
#include <ISTE/Scene/Scene.h>

#include "ISTE/Graphics/RenderStateManager.h"

struct ObjectBuffer
{
	CU::Matrix4x4f myTransform;
};

bool ISTE::ModelShader::CreateBuffers()
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

void ISTE::ModelShader::Render(EntityID aEntity)
{
	ModelCommand com = PrepEntity(aEntity);
	BindObjBuffer(com);  


	myCtx->myTextureManager->PsBindTexture(com.myTextureIDs[ALBEDO_MAP], 0);
	myCtx->myTextureManager->PsBindTexture(com.myTextureIDs[NORMAL_MAP], 1);
	myCtx->myTextureManager->PsBindTexture(com.myTextureIDs[MATERIAL_MAP], 2);

	myCtx->myModelManager->RenderModel(com.myModelId);
} 

ISTE::ModelCommand ISTE::ModelShader::PrepEntity(EntityID aEntity)
{
	ModelCommand modelRC;

	{
	ModelComponent* modelData = myCtx->mySceneHandler->GetActiveScene().GetComponent<ModelComponent>(aEntity); 

	modelRC.myModelId = modelData->myModelId;
	modelRC.myColor = { modelData->myColor.x,modelData->myColor.y,modelData->myColor.z, 1 };
	modelRC.myUV = modelData->myUV;
	modelRC.myUVScale = modelData->myUVScale;
	modelRC.mySamplerState = modelData->mySamplerState;
	modelRC.myAdressMode = modelData->myAdressMode;
	myCtx->myRenderStateManager->SetSampleState(modelData->myAdressMode, modelData->mySamplerState);
	}

	{
	MaterialComponent* matComp = myCtx->mySceneHandler->GetActiveScene().GetComponent<MaterialComponent>(aEntity);
	memcpy(modelRC.myTextureIDs, matComp->myTextures, sizeof(TextureID) * MAX_MATERIAL_COUNT);
	}
	
	{
	TransformComponent* transformData = myCtx->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(aEntity);
	modelRC.myTransform = transformData->myCachedTransform; 
	}

	return modelRC;
}

void ISTE::ModelShader::BindObjBuffer(const ModelCommand& aCommand)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	HRESULT res = context->Map(myObjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	ObjectBuffer* OBdata = (ObjectBuffer*)(resource.pData);

	OBdata->myTransform = aCommand.myTransform;
	context->Unmap(myObjBuffer.Get(), 0);
	context->VSSetConstantBuffers(2, 1, myObjBuffer.GetAddressOf());
	context->PSSetConstantBuffers(2, 1, myObjBuffer.GetAddressOf());
}
