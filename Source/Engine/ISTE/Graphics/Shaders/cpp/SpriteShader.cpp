#include "SpriteShader.h"
#include <d3d11.h>

#include "ISTE/Context.h"
#include "ISTE/Graphics/DX11.h"

#include <ISTE/Scene/SceneHandler.h>
#include <ISTE/Scene/Scene.h>
#include <ISTE/Graphics/ComponentAndSystem/TransformComponent.h> 
#include <ISTE/Graphics/ComponentAndSystem/TransperancyComponent.h>
#include "ISTE/Graphics/ComponentAndSystem/Sprite3DCommand.h"
#include "ISTE/Graphics/ComponentAndSystem/Sprite3DComponent.h"
#include "ISTE/Graphics/RenderStateManager.h"

#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/Resources/ShaderManager.h"


struct ObjectBuffer {
	CU::Matrix4x4f	myTransformMatrix;
	CU::Vec4f		myColor;

	CU::Vec2f		myUVStart;
	CU::Vec2f		myUVEnd;
	CU::Vec2f		myUVOffset;
	CU::Vec2f		myUVScale;
};


ISTE::Sprite3DShader::Sprite3DShader()
{
}

ISTE::Sprite3DShader::~Sprite3DShader()
{
}

bool ISTE::Sprite3DShader::CreateBuffers()
{
	mySpriteQuad = myCtx->myModelManager->LoadSpriteQuad();

	//FrameBuffer init
	{
		//move out in to a shader class later
		D3D11_BUFFER_DESC objBufferDesc = {};
		objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		objBufferDesc.ByteWidth = sizeof(ObjectBuffer);
		objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		HRESULT result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myObjBuffer.GetAddressOf());
		if (FAILED(result))
			return false;
	}
	return true;
}

void ISTE::Sprite3DShader::Render(EntityID aEntity)
{
	ISTE::Sprite3DRenderCommand com = PrepEntity(aEntity);

	//obj buffer
	BindObjBuffer(com);

	//sampler state
	myCtx->myRenderStateManager->SetSampleState(com.myAdressMode, com.mySamplerState);

	//sets texture
	myCtx->myTextureManager->PsBindTexture(com.myTextureId, 0);

	//end stuff
	myCtx->myModelManager->RenderMesh(mySpriteQuad, 0);
}

ISTE::Sprite3DRenderCommand ISTE::Sprite3DShader::PrepEntity(EntityID aEntity)
{
	Sprite3DRenderCommand SpriteRenderCommand;
	
	Sprite3DComponent* spriteData = myCtx->mySceneHandler->GetActiveScene().GetComponent<Sprite3DComponent>(aEntity);
	TransformComponent* transformData = myCtx->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(aEntity);
	
	//processes sprite data
	{ 
		SpriteRenderCommand.myTextureId = spriteData->myTextureId;
		SpriteRenderCommand.myColor = { spriteData->myColor.x,spriteData->myColor.y,spriteData->myColor.z,1 };
		SpriteRenderCommand.myUVStart = spriteData->myUVStart;
		SpriteRenderCommand.myUVEnd = spriteData->myUVEnd;
		SpriteRenderCommand.myUVOffset = spriteData->myUVOffset;
		SpriteRenderCommand.myUVScale = spriteData->myUVScale;
		SpriteRenderCommand.mySamplerState = spriteData->mySamplerState;
		SpriteRenderCommand.myAdressMode = spriteData->myAdressMode;
	}

	//processses transform data
	{ 
		SpriteRenderCommand.myTransform = transformData->myCachedTransform;
	}  
	return SpriteRenderCommand;
}
 

void ISTE::Sprite3DShader::BindObjBuffer(const ISTE::Sprite3DRenderCommand& aCommand)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	HRESULT res = context->Map(myObjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	ObjectBuffer* OBdata = (ObjectBuffer*)(resource.pData);

	OBdata->myTransformMatrix = aCommand.myTransform;
	OBdata->myColor				= aCommand.myColor;
	OBdata->myUVOffset			= aCommand.myUVOffset;
	OBdata->myUVStart			= aCommand.myUVStart;
	OBdata->myUVEnd				= aCommand.myUVEnd;
	OBdata->myUVScale			= aCommand.myUVScale;
	context->Unmap(myObjBuffer.Get(), 0);
	context->VSSetConstantBuffers(2, 1, myObjBuffer.GetAddressOf());
	context->PSSetConstantBuffers(2, 1, myObjBuffer.GetAddressOf());
}
