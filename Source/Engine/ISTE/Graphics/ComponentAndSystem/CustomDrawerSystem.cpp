#include <ISTE/Graphics/ComponentAndSystem/CustomDrawerSystem.h>
#include <ISTE/Graphics/ComponentAndSystem/CustomShaderComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/TransformComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/ModelComponent.h>
#include <ISTE/Context.h>
#include <ISTE/Graphics/Resources/ShaderManager.h>
#include <ISTE/Scene/SceneHandler.h>
#include <ISTE/Scene/Scene.h>
#include <ISTE/Graphics/RenderStateManager.h>

#include <d3d11.h>
#include <ISTE/Graphics/DX11.h>

struct VSObjectBufferVFX
{
	CU::Matrix4x4f myTransform;
};
struct PSObjectBufferVFX
{
	CU::Vec4f myColor;

	CU::Vec2f myUvChangeOverTime0;
	CU::Vec2f myUvChangeOverTime1;
	CU::Vec2f myUvChangeOverTime2;
	CU::Vec2f _g0;

	CU::Vec3f mySample0;
	float _g1;
	CU::Vec3f mySample1;
	float _g2;
	CU::Vec3f mySample2;
	float _g3;

	float myElaspedTime;
	float myDuration;
	float myDistToCamera;
	float _g4;
};


bool ISTE::CustomDrawerSystem::Init()
{
	myCtx = Context::Get();

	HRESULT result;
	{
		D3D11_BUFFER_DESC objBufferDesc = {};
		objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		objBufferDesc.ByteWidth = sizeof(VSObjectBufferVFX);
		objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myVsObjBuffer.GetAddressOf());
		if (FAILED(result))
			return false;
	}
	{
		D3D11_BUFFER_DESC objBufferDesc = {};
		objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		objBufferDesc.ByteWidth = sizeof(PSObjectBufferVFX);
		objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myPsObjBuffer.GetAddressOf());
		if (FAILED(result))
			return false;
	}
	return true;
}

void ISTE::CustomDrawerSystem::PrepareRenderCommands()
{
	//ShaderManager* sm = myCtx->myShaderManager;
	SceneHandler* sceneHandler = myCtx->mySceneHandler;
	Scene& currentScene = myCtx->mySceneHandler->GetActiveScene();


	CustomShaderComponent* shaderComponent;
	TransformComponent* transformComponent;

	for (EntityID entity : myEntities[0])
	{
		shaderComponent = currentScene.GetComponent<CustomShaderComponent>(entity);
		transformComponent = currentScene.GetComponent<TransformComponent>(entity);
		ModelComponent* modelComponent = currentScene.GetComponent<ModelComponent>(entity);

		RenderCommand rc;
		rc.myEntity = entity;
		rc.myShader = shaderComponent->myShader;
		rc.myAdressMode = modelComponent->myAdressMode;
		rc.mySamplerState = modelComponent->mySamplerState;
		rc.myTransform = transformComponent->myCachedTransform;
		rc.myColor.x = modelComponent->myColor.x;
		rc.myColor.y = modelComponent->myColor.y;
		rc.myColor.z = modelComponent->myColor.z;
		rc.myColor.w = 0.f; // debug alpha
		rc.myUvChangeOverTime0 = shaderComponent->myUvChangeOverTimes[0];
		rc.myUvChangeOverTime1 = shaderComponent->myUvChangeOverTimes[1];
		rc.myUvChangeOverTime2 = shaderComponent->myUvChangeOverTimes[2];
		rc.mySample0 = shaderComponent->mySamples[0];
		rc.mySample1 = shaderComponent->mySamples[1];
		rc.mySample2 = shaderComponent->mySamples[2];
		rc.myElaspedTime = shaderComponent->myElaspedTime;
		rc.myDuration = shaderComponent->myDuration;
		rc.myDistToCamera = (transformComponent->myPosition - sceneHandler->GetCamera().GetPosition()).Length();

		myRenderCommands.Enqueue(rc);

		//sm->BindShader(shaderComponent->myShader);
		//sm->Draw(shaderComponent->myShader, entity);
	}
};

void ISTE::CustomDrawerSystem::Draw()
{
	myCtx->myRenderStateManager->SetDepthState(ISTE::DepthState::eDepthLess, ISTE::ReadWriteState::eRead);
	myCtx->myRenderStateManager->SetRasterState(ISTE::RasterizerState::eNoFaceCulling);
	myCtx->myRenderStateManager->SetBlendState(ISTE::BlendState::eAlphaBlend);
	
	ShaderManager* sm = myCtx->myShaderManager;
	size_t size = myRenderCommands.GetSize();
	for (; size != 0; size--)
	{
		RenderCommand rc = myRenderCommands.Dequeue();
		myCtx->myRenderStateManager->SetSampleState(rc.myAdressMode, rc.mySamplerState);
		sm->BindShader(rc.myShader);
		BindObjBuffer(rc);
		sm->Draw(rc.myShader, rc.myEntity);
	}
	myCtx->myRenderStateManager->SetDepthState(ISTE::DepthState::eDepthLess, ISTE::ReadWriteState::eWrite);
}

void ISTE::CustomDrawerSystem::BindObjBuffer(const RenderCommand& aCommand)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	HRESULT res = context->Map(myVsObjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	{
		VSObjectBufferVFX* OBdata = (VSObjectBufferVFX*)(resource.pData);

		OBdata->myTransform = aCommand.myTransform;
		context->Unmap(myVsObjBuffer.Get(), 0);
		context->VSSetConstantBuffers(2, 1, myVsObjBuffer.GetAddressOf());
	}
	resource = { 0 };
	res = context->Map(myPsObjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	{
		PSObjectBufferVFX* OBdata = (PSObjectBufferVFX*)(resource.pData);

		OBdata->myColor = aCommand.myColor;
		OBdata->myUvChangeOverTime0 = aCommand.myUvChangeOverTime0;
		OBdata->myUvChangeOverTime1 = aCommand.myUvChangeOverTime1;
		OBdata->myUvChangeOverTime2 = aCommand.myUvChangeOverTime2;
		OBdata->mySample0 = aCommand.mySample0;
		OBdata->mySample1 = aCommand.mySample1;
		OBdata->mySample2 = aCommand.mySample2;
		OBdata->myElaspedTime = aCommand.myElaspedTime;
		OBdata->myDuration = aCommand.myDuration;
		OBdata->myDistToCamera = aCommand.myDistToCamera;
		context->Unmap(myPsObjBuffer.Get(), 0);
		context->PSSetConstantBuffers(3, 1, myPsObjBuffer.GetAddressOf());
	}
}
