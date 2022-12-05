#include "VFXSpriteDrawerSystem.h"
#include <d3d11.h>
#include <fstream>

#include <ISTE/Context.h>
#include <ISTE/Graphics/DX11.h>

#include <ISTE/Scene/SceneHandler.h>
#include <ISTE/Scene/Scene.h>
#include <ISTE/Graphics/ComponentAndSystem/TransformComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/VFXSprite3DCommand.h>
#include <ISTE/Graphics/ComponentAndSystem/VFXSprite2DCommand.h>
#include <ISTE/Graphics/ComponentAndSystem/VFXSprite3DComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/VFXSprite2DComponent.h>
#include <ISTE/Graphics/GraphicsEngine.h>
#include <ISTE/Graphics/Resources/ModelManager.h>
#include <ISTE/Graphics/Resources/TextureManager.h>
#include <ISTE/Graphics/Resources/ShaderManager.h>
#include "ISTE/Graphics/RenderStateManager.h"

struct VFXSprite3DObjectBuffer {
	CU::Matrix4x4f	myTransformMatrix;

	CU::Vec2f		myUVStart;
	CU::Vec2f		myUVEnd;
	CU::Vec2f		myUVOffset;
	CU::Vec2f		myUVScale;

	CU::Vec4f		myColor;
};

struct VFXSprite2DObjectBuffer {
	CU::Vec2f		myPosition; 
	CU::Vec2f		myScale; 
	float			myRot;
	CU::Vec3f garb;

	CU::Vec2f		myUVStart;
	CU::Vec2f		myUVEnd;
	CU::Vec2f		myUVOffset;
	CU::Vec2f		myUVScale; 
};

ISTE::VFXSpriteDrawerSystem::~VFXSpriteDrawerSystem()
{
}

bool ISTE::VFXSpriteDrawerSystem::Init()
{
	myCtx = Context::Get();
	mySceneHandler = myCtx->mySceneHandler;
	mySpriteQuad = myCtx->myModelManager->LoadSpriteQuad(); 

	if (!InitShader("Shaders/VFX3DSprite_VS.cso", "Shaders/VFX3DSprite_PS.cso", VFXSpriteComplexity::eVFX3DSprite))
		return false;
	//if (!InitShader("Shaders/VFX2DSprite_VS.cso", "Shaders/VFX2DSprite_PS.cso", VFXSpriteComplexity::eVFX2DSprite))
	//	return false;

	//if (!Init2DSpriteBuffer())
	//	return false;
	if (!Init3DSpriteBuffer())
		return false;
	return true;
}

void ISTE::VFXSpriteDrawerSystem::PrepareRenderCommands()
{
	Prep3DSprites();
	Prep2DSprites();
}

void ISTE::VFXSpriteDrawerSystem::Draw()
{
	myCtx->myModelManager->BindMesh(mySpriteQuad,0);
	Render2DSprites();
	Render3DSprites();
}

void ISTE::VFXSpriteDrawerSystem::DrawDepth()
{
	Render2DSprites();
	Render3DSprites();
}

void ISTE::VFXSpriteDrawerSystem::ClearCommands()
{
	my2DRenderCommands.clear();
	my3DRenderCommands.clear();
}

void ISTE::VFXSpriteDrawerSystem::Add2DSpriteRenderCommand(VFXSprite2DRenderCommand aCmd)
{
	my2DRenderCommands.push_back(aCmd);
}

void ISTE::VFXSpriteDrawerSystem::Add3DSpriteRenderCommand(VFXSprite3DRenderCommand aCmd)
{
	my3DRenderCommands.push_back(aCmd);
}

void ISTE::VFXSpriteDrawerSystem::Render2DSprites()
{
	myCtx->myModelManager->BindMesh(mySpriteQuad, 0);
	BindShader(VFXSpriteComplexity::eVFX2DSprite);
	size_t size = my2DRenderCommands.size();
	for (int i = 0; i < size; i++)
	{
		VFXSprite2DRenderCommand com = my2DRenderCommands[i];

		//binds obj buffer
		BindSprite2DObjBuffer(com);

		//sampler state
		myCtx->myRenderStateManager->SetSampleState(com.myAdressMode, com.mySamplerState);

		//sets texture
		myCtx->myTextureManager->PsBindTexture(com.myTextureId, 0);

		//end stuff
		myCtx->myModelManager->RenderMesh(mySpriteQuad, 0);
	}
}

void ISTE::VFXSpriteDrawerSystem::Render3DSprites()
{
	myCtx->myRenderStateManager->SetDepthState(ISTE::DepthState::eDepthLess, ISTE::ReadWriteState::eRead);
	myCtx->myRenderStateManager->SetRasterState(ISTE::RasterizerState::eNoFaceCulling);
	myCtx->myRenderStateManager->SetBlendState(ISTE::BlendState::eAlphaBlend);

	myCtx->myModelManager->BindMesh(mySpriteQuad, 0);
	BindShader(VFXSpriteComplexity::eVFX3DSprite);
	size_t size = my3DRenderCommands.size();
	for (int i = 0; i < size; i++)
	{
		VFXSprite3DRenderCommand com = my3DRenderCommands[i];

		//binds obj buffer
		BindSprite3DObjBuffer(com);

		//sampler state
		myCtx->myRenderStateManager->SetSampleState(com.myAdressMode, com.mySamplerState);

		//sets texture
		myCtx->myTextureManager->PsBindTexture(com.myTextureId, 0);

		//end stuff
		myCtx->myModelManager->RenderMesh(mySpriteQuad, 0);
	}
	myCtx->myRenderStateManager->SetDepthState(ISTE::DepthState::eDepthLess, ISTE::ReadWriteState::eWrite);
}

void ISTE::VFXSpriteDrawerSystem::Prep3DSprites()
{
	Scene& scene = mySceneHandler->GetActiveScene();
	ComponentPool& transformPool	= scene.GetComponentPool<TransformComponent>();
	ComponentPool& sprite3DPool		= scene.GetComponentPool<VFXSprite3DComponent>();
	for (auto entity : myEntities[(int)VFXSpriteComplexity::eVFX3DSprite])
	{
		VFXSprite3DRenderCommand SpriteRenderCommand;
		//processes sprite data
		{
			VFXSprite3DComponent* spriteData = (VFXSprite3DComponent*)sprite3DPool.Get(GetEntityIndex(entity));
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
			TransformComponent* transformData = (TransformComponent*)transformPool.Get(GetEntityIndex(entity));
			SpriteRenderCommand.myTransform = transformData->myCachedTransform;
		}

		my3DRenderCommands.emplace_back(SpriteRenderCommand);
	}
	// Sorting
	const CU::Vec3f cameraPos = myCtx->mySceneHandler->GetCamera().GetPosition();
	std::sort(begin(my3DRenderCommands), end(my3DRenderCommands), 
		[cameraPos](VFXSprite3DRenderCommand& v0, VFXSprite3DRenderCommand& v1)
		{
			return (v0.myTransform.GetTranslationV3() - cameraPos).LengthSqr() > (v1.myTransform.GetTranslationV3() - cameraPos).LengthSqr();
		});
	//// Rotation and positioning towards camera
	//const CU::Vec3f cameraUp = myCtx->mySceneHandler->GetCamera().GetTransform().GetUp();
	////const CU::Vec3f cameraUp = {0, 1, 0};
	//for (int i = 0; i < my3DRenderCommands.size(); i++)
	//{
	//	auto& com = my3DRenderCommands[i];
	//	CU::Vec3f pos = com.myTransform.GetPos();
	//	CU::Vec3f forwardDir = (pos - cameraPos).GetNormalized();
	//	CU::Vec3f rightDir = forwardDir.Cross(-1.f * cameraUp).GetNormalized();
	//	CU::Vec3f upDir = forwardDir.Cross(rightDir).GetNormalized();
	//	float scale = com.myTransform.GetScale().z; // sprites transform scale.z is used for the offset towards the camera // Mathias
	//	com.myTransform.SetPosition(pos - scale * forwardDir);
	//	com.myTransform.SetForward(scale * forwardDir);
	//	com.myTransform.SetRight(scale * rightDir);
	//	com.myTransform.SetUp(scale * upDir);
	//}
}

void ISTE::VFXSpriteDrawerSystem::Prep2DSprites()
{
	Scene& scene = mySceneHandler->GetActiveScene();
	ComponentPool& sprite2DPool = scene.GetComponentPool<VFXSprite2DComponent>();
	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
	for (auto entity : myEntities[(int)VFXSpriteComplexity::eVFX2DSprite])
	{
		VFXSprite2DRenderCommand SpriteRenderCommand;
		//processes sprite data
		{
			VFXSprite2DComponent* spriteData = (VFXSprite2DComponent*)sprite2DPool.Get(GetEntityIndex(entity));
			if (spriteData->myTextureId == -1)
				continue;

			SpriteRenderCommand.myTextureId		= spriteData->myTextureId;
			SpriteRenderCommand.myColor			= { spriteData->myColor.x,spriteData->myColor.y,spriteData->myColor.z,1 };
			SpriteRenderCommand.myUVStart		= spriteData->myUVStart;
			SpriteRenderCommand.myUVEnd			= spriteData->myUVEnd;
			SpriteRenderCommand.myUVOffset		= spriteData->myUVOffset;
			SpriteRenderCommand.myUVScale		= spriteData->myUVScale;
			SpriteRenderCommand.myUVScale		= spriteData->myUVScale;
			SpriteRenderCommand.mySamplerState	= spriteData->mySamplerState;
			SpriteRenderCommand.myAdressMode	= spriteData->myAdressMode;
		}

		//processses transform data
		{
			TransformComponent* transformData = (TransformComponent*)transformPool.Get(GetEntityIndex(entity));
			SpriteRenderCommand.myPosition.x	= transformData->myPosition.x;
			SpriteRenderCommand.myPosition.y	= transformData->myPosition.y;

			SpriteRenderCommand.myScale.x		= transformData->myScale.x;
			SpriteRenderCommand.myScale.y		= transformData->myScale.y;

			SpriteRenderCommand.myRotation		= transformData->myEuler.GetAngles().z; 
		}
		my2DRenderCommands.emplace_back(SpriteRenderCommand);
	}
}

void ISTE::VFXSpriteDrawerSystem::BindSprite3DObjBuffer(const VFXSprite3DRenderCommand& aCommand)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	HRESULT res = context->Map(myObjBuffer[(int)VFXSpriteComplexity::eVFX3DSprite].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	VFXSprite3DObjectBuffer* OBdata = (VFXSprite3DObjectBuffer*)(resource.pData);

	OBdata->myTransformMatrix = aCommand.myTransform; 

	OBdata->myUVOffset	= aCommand.myUVOffset;
	OBdata->myUVStart	= aCommand.myUVStart;
	OBdata->myUVEnd		= aCommand.myUVEnd;
	OBdata->myUVScale	= aCommand.myUVScale;
	OBdata->myColor		= aCommand.myColor;

	context->Unmap(myObjBuffer[(int)VFXSpriteComplexity::eVFX3DSprite].Get(), 0);


	context->VSSetConstantBuffers(2, 1, myObjBuffer[(int)VFXSpriteComplexity::eVFX3DSprite].GetAddressOf());
	context->PSSetConstantBuffers(2, 1, myObjBuffer[(int)VFXSpriteComplexity::eVFX3DSprite].GetAddressOf());
}

void ISTE::VFXSpriteDrawerSystem::BindSprite2DObjBuffer(const VFXSprite2DRenderCommand& aCommand)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	HRESULT res = context->Map(myObjBuffer[(int)VFXSpriteComplexity::eVFX2DSprite].Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	VFXSprite2DObjectBuffer* OBdata = (VFXSprite2DObjectBuffer*)(resource.pData);
	OBdata->myPosition	= aCommand.myPosition;
	OBdata->myRot		= aCommand.myRotation;
	OBdata->myScale		= aCommand.myScale; 
	OBdata->myUVStart	= aCommand.myUVStart;
	OBdata->myUVEnd		= aCommand.myUVEnd;
	OBdata->myUVOffset	= aCommand.myUVOffset;
	OBdata->myUVScale	= aCommand.myUVScale;
	context->Unmap(myObjBuffer[(int)VFXSpriteComplexity::eVFX2DSprite].Get(), 0);


	context->VSSetConstantBuffers(2, 1, myObjBuffer[(int)VFXSpriteComplexity::eVFX2DSprite].GetAddressOf());
	context->PSSetConstantBuffers(2, 1, myObjBuffer[(int)VFXSpriteComplexity::eVFX2DSprite].GetAddressOf());
}

void ISTE::VFXSpriteDrawerSystem::BindShader(VFXSpriteComplexity aComplexity)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetShader(myVertexShader[(int)aComplexity].Get(), 0, 0);
	context->PSSetShader(myPixelShader[(int)aComplexity].Get(), 0, 0);
	context->IASetInputLayout(myInputLayout[(int)aComplexity].Get());
}

bool ISTE::VFXSpriteDrawerSystem::InitShader(std::string aVSPath, std::string aPSPath, VFXSpriteComplexity aSpriteComplexity)
{
	ID3D11Device* aDevice = myCtx->myDX11->GetDevice();
	HRESULT result;
	std::string vsData;
	DX11::CreateVertexShader(aVSPath, myVertexShader[(int)aSpriteComplexity].GetAddressOf(), vsData);
	DX11::CreatePixelShader(aPSPath, myPixelShader[(int)aSpriteComplexity].GetAddressOf());
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
		result = aDevice->CreateInputLayout(layout, elementCount, vsData.data(), vsData.size(), myInputLayout[(int)aSpriteComplexity].GetAddressOf());
		if (FAILED(result))
			return false;
	}
	return true;
}

bool ISTE::VFXSpriteDrawerSystem::Init2DSpriteBuffer()
{
	D3D11_BUFFER_DESC objBufferDesc = {};
	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objBufferDesc.ByteWidth = sizeof(VFXSprite2DObjectBuffer);
	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myObjBuffer[(int)VFXSpriteComplexity::eVFX2DSprite].GetAddressOf());
	if (FAILED(result))
		return false;

	return true;
}

bool ISTE::VFXSpriteDrawerSystem::Init3DSpriteBuffer()
{
	D3D11_BUFFER_DESC objBufferDesc = {};
	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objBufferDesc.ByteWidth = sizeof(VFXSprite3DObjectBuffer);
	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	HRESULT result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myObjBuffer[(int)VFXSpriteComplexity::eVFX3DSprite].GetAddressOf());
	if (FAILED(result))
		return false;

	return true;
}