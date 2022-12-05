#include "TextSystem.h"
#include <d3d11.h>
#include <iostream>

#include "ISTE/Context.h"
#include "ISTE/Graphics/DX11.h"
#include "ISTE/WindowsWindow.h"

#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Text/TextService.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Text/TextService.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Text/TextRenderCommand.h"
#include "ISTE/ECSB/ECSDefines.hpp"

#include "TextComponent.h"
#include "ISTE/Graphics/RenderStateManager.h"
#include "ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h"

struct ObjBuffer {
	CU::Vec2f	myPosition;
	CU::Vec2f	myScale;
	float		myRot = 0;
	CU::Vec3f garb;

	CU::Vec2f	myUVStart;
	CU::Vec2f	myUVEnd;
	CU::Vec2f	myUVOffset = { 0, 0 };
	CU::Vec2f	myUVScale = { 1, 1 };
	CU::Vec4f	myColor = {1.f,1.f,1.f,1.f};

	//CU::Vec2f myPosition;
	//CU::Vec2f mySize;
	//CU::Vec2f myUvStart;
	//CU::Vec2f myUvEnd;
	//
	//CU::Vec3f myColor;
	//float garb;
};



ISTE::TextSystem::TextSystem()
{
}

ISTE::TextSystem::~TextSystem()
{
}

bool ISTE::TextSystem::Init()
{
	myCtx = Context::Get();

	myQuadId = myCtx->myModelManager->LoadSpriteQuad();

	if (!InitShaders())
	{
		std::cout << "Failed to init TextSystems Shader.\n";
		return false;
	} 
	if (!CreateBuffers())
	{
		std::cout << "Failed to create TextSystems Buffers.\n";
		return false;
	}

	return true;
}

void ISTE::TextSystem::AddTextCommand(const TextRenderCommand& command)
{
	SpriteDrawerSystem* sds = myCtx->mySystemManager->GetSystem<SpriteDrawerSystem>(); 
	 
	auto& fonts = myCtx->myTextureManager->GetTextService()->GetFontList(); 
	CU::Vec2Ui res = myCtx->myWindow->GetResolution();


	float screenSizeX = static_cast<float>(res.x);
	float screenSizeY = static_cast<float>(res.y);
	float screenScaleRatioX = screenSizeX / 1280.f;
	float screenScaleRatioY = screenSizeY /  720.f;

	float winRatio = (screenSizeX / screenSizeY); 

	TextService::Font* fontData = fonts.GetByIterator(command.myFontID);
	if (fontData == nullptr)
		return;

	int count = (int)command.myText.size();
	const char* str = command.myText.c_str();
	float scale = command.myTextScale;
	float rotation = command.myRotation * ISTE::DegToRad;


	float midX = 0;
	float midY = 0;
	if (rotation != 0.0f)
	{
		float minX = FLT_MAX;
		float minY = FLT_MAX;
		float maxX = FLT_MIN;
		float maxY = FLT_MIN;

		float drawX = 0.f;
		float drawY = 0.f;
		float maxDrawY = 0.f;

		for (int i = 0; i < count; i++)
		{
			Sprite2DRenderCommand charInfo = myCtx->myTextureManager->GetTextService()->processNextCharacter(*fontData, str[i], scale, drawX, drawY, maxDrawY, command.myWordSpacing, command.myLetterSpacing, command.myLineSpacing);

			minX = min(minX, charInfo.myPosition.x);
			minY = min(minY, charInfo.myPosition.y);
			maxX = max(maxX, charInfo.myPosition.x + charInfo.myScale.x);
			maxY = max(maxY, charInfo.myPosition.y + charInfo.myScale.y);
		}

		midX = 0.5f * (maxX + minX) / screenSizeX;
		midY = 0.5f * (maxY + minY) / screenSizeY;
	}



	Sprite2DRenderCommand renderCommand = {};
	if (fontData->myFontHeightWidth < 18)
	{
		renderCommand.mySamplerState = SamplerState::ePoint;
	}
	else
	{
		renderCommand.mySamplerState = SamplerState::eBiliniear;
	}

	renderCommand.myPosition.x = 0;
	renderCommand.myPosition.y = 0;

	float drawX = 0.f;
	float drawY = 0.f;
	float maxDrawY = 0.f;

	float diffX = midX;
	float diffY = midY;

	float c = 0.f, s = 0.f;
	if (rotation != 0.0f)
	{
		c = cos(rotation);
		s = sin(rotation);
	}

	for (int i = 0; i < count; i++)
	{
		Sprite2DRenderCommand charInfo = myCtx->myTextureManager->GetTextService()->processNextCharacter(*fontData, str[i], scale, drawX, drawY, maxDrawY, command.myWordSpacing, command.myLetterSpacing, command.myLineSpacing);

		charInfo.myPosition.x /= screenSizeX;
		charInfo.myPosition.y /= screenSizeY;

		charInfo.myScale.x /= screenSizeX;
		charInfo.myScale.y /= screenSizeY;

		// charInfo.mySize.x *= (screenSizeY /screenSizeX);

		if (rotation != 0.0f)
		{
			renderCommand.myRotation = rotation;

			float x = charInfo.myPosition.x - diffX;
			float y = charInfo.myPosition.y - diffY;

			y /= winRatio;

			charInfo.myPosition.x = x * c - y * s + diffX;
			charInfo.myPosition.y = (x * s + y * c) * winRatio + diffY;
		}


		renderCommand.myPosition.x = command.myPosition.x + charInfo.myPosition.x;
		renderCommand.myPosition.y = command.myPosition.y + charInfo.myPosition.y;

		renderCommand.myScale.x			= charInfo.myScale.x * scale * screenScaleRatioX;
		renderCommand.myScale.y			= charInfo.myScale.y * scale * screenScaleRatioY;
		renderCommand.myUVStart			= { charInfo.myUVStart.x, charInfo.myUVStart.y };
		renderCommand.myUVEnd			= { charInfo.myUVEnd.x, charInfo.myUVEnd.y };
		renderCommand.myTextureId[0]	= fontData->myFontAtlasID;
		renderCommand.myColor			= command.myColor;
		renderCommand.myPivot			= { 0.5,-0.5 };

		sds->Add2DSpriteRenderCommand(renderCommand);
	}
}


void ISTE::TextSystem::PrepareRenderCommands()
{
	SpriteDrawerSystem* sds = myCtx->mySystemManager->GetSystem<SpriteDrawerSystem>();
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();

	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
	ComponentPool& textPool = scene.GetComponentPool<TextComponent>();
	auto& fonts = myCtx->myTextureManager->GetTextService()->GetFontList();

	CU::Vec2Ui res = myCtx->myWindow->GetResolution();

	float screenSizeX = static_cast<float>(res.x);
	float screenSizeY = static_cast<float>(res.y);
	float screenScaleRatioX = screenSizeX / 1280.f;
	float screenScaleRatioY = screenSizeY / 720.f;

	float winRatio = (screenSizeX / screenSizeY);


	for (auto& entity : myEntities[0])
	{
		TextComponent* textComp = (TextComponent*)textPool.Get(GetEntityIndex(entity));
		TransformComponent* transformComp = (TransformComponent*)transformPool.Get(GetEntityIndex(entity));

		TextService::Font* fontData = fonts.GetByIterator(textComp->myFontID);
		if (fontData == nullptr) 
			continue; 

		int count = (int)textComp->myText.size();
		const char* str = textComp->myText.c_str();
		float scale = textComp->myScale;
		float rotation = transformComp->myEuler.GetAngles().z * ISTE::DegToRad;


		float midX = 0;
		float midY = 0;
		if (rotation != 0.0f)
		{
			float minX = FLT_MAX;
			float minY = FLT_MAX;
			float maxX = FLT_MIN;
			float maxY = FLT_MIN;

			float drawX = 0.f;
			float drawY = 0.f;
			float maxDrawY = 0.f;

			for (int i = 0; i < count; i++)
			{
				Sprite2DRenderCommand charInfo = myCtx->myTextureManager->GetTextService()->processNextCharacter(*fontData, str[i], scale, drawX, drawY, maxDrawY, textComp->myWordSpacing, textComp->myLetterSpacing, textComp->myLineSpacing);

				minX = min(minX, charInfo.myPosition.x);
				minY = min(minY, charInfo.myPosition.y);
				maxX = max(maxX, charInfo.myPosition.x + charInfo.myScale.x);
				maxY = max(maxY, charInfo.myPosition.y + charInfo.myScale.y);
			}

			midX = 0.5f * (maxX + minX) / screenSizeX;
			midY = 0.5f * (maxY + minY) / screenSizeY;
		}



		Sprite2DRenderCommand renderCommand = {};
		if (fontData->myFontHeightWidth < 18)
		{
			renderCommand.mySamplerState = SamplerState::ePoint;
		}
		else
		{
			renderCommand.mySamplerState = SamplerState::eBiliniear;
		}

		renderCommand.myPosition.x = 0;
		renderCommand.myPosition.y = 0;

		float drawX = 0.f;
		float drawY = 0.f;
		float maxDrawY = 0.f;

		float diffX = midX;
		float diffY = midY;

		float c = 0.f, s = 0.f;  
		if (rotation != 0.0f)
		{
			c = cos(rotation);
			s = sin(rotation);
		}

		for (int i = 0; i < count; i++)
		{
			Sprite2DRenderCommand charInfo = myCtx->myTextureManager->GetTextService()->processNextCharacter(*fontData, str[i], scale, drawX, drawY, maxDrawY, textComp->myWordSpacing, textComp->myLetterSpacing, textComp->myLineSpacing);

			charInfo.myPosition.x /= screenSizeX;
			charInfo.myPosition.y /= screenSizeY; 

			charInfo.myScale.x /= screenSizeX;
			charInfo.myScale.y /= screenSizeY; 

			//charInfo.myScale.x *= (screenSizeY /screenSizeX);

			if (rotation != 0.0f)
			{
				renderCommand.myRotation = rotation;
			
				float x = charInfo.myPosition.x - diffX;
				float y = charInfo.myPosition.y - diffY;
			
				y /= winRatio;
			
				charInfo.myPosition.x = x * c - y * s + diffX;
				charInfo.myPosition.y = (x * s + y * c) * winRatio + diffY;
			}
			
			renderCommand.myPosition.x		= transformComp->myPosition.x + charInfo.myPosition.x;
			renderCommand.myPosition.y		= transformComp->myPosition.y + charInfo.myPosition.y;

			renderCommand.myScale.x			= charInfo.myScale.x * scale * screenScaleRatioX;
			renderCommand.myScale.y			= charInfo.myScale.y * scale * screenScaleRatioY;
			renderCommand.myUVStart			= { charInfo.myUVStart.x, charInfo.myUVStart.y};
			renderCommand.myUVEnd			= { charInfo.myUVEnd.x, charInfo.myUVEnd.y };
			renderCommand.myTextureId[0]	= fontData->myFontAtlasID;
			renderCommand.myColor			= textComp->myColor; 
			renderCommand.myPivot			= {0.5,-0.5};

			sds->Add2DSpriteRenderCommand(renderCommand);
		}
	}
}

void ISTE::TextSystem::Draw()
{
	//myCtx->myModelManager->BindMesh(myQuadId, 0);
	//BindShader();
	//
	//myCtx->myRenderStateManager->SetBlendState(BlendState::eAlphaBlend);
	//
	//size_t size = myRenderCommands.size();
	//TextRenderCommand com;
	//for (int i = 0; i < size; i++)
	//{
	//	com = myRenderCommands[i];
	//
	//	//binds obj buffer
	//	BindBuffers(com);
	//
	//	//sampler state
	//	myCtx->myRenderStateManager->SetSampleState(AdressMode::eWrap, com.mySamplerState);
	//
	//	//sets texture
	//	myCtx->myTextService->PsBindFontTexture(com.myFontId, 0);
	//
	//	//end stuff
	//	myCtx->myModelManager->RenderMesh(myQuadId, 0);
	//}
	//
	//myCtx->myRenderStateManager->SetBlendState(BlendState::eDisabled);
}

void ISTE::TextSystem::ClearCommands()
{
	//myRenderCommands.clear();
}

bool ISTE::TextSystem::InitShaders()
{
	ID3D11Device* aDevice = myCtx->myDX11->GetDevice();
	HRESULT result; 
	std::string vsData;
	//ISTE::DX11::CreateVertexShader("Shaders/2DSprite_VS.cso", myVertexShader.GetAddressOf(), vsData);
	ISTE::DX11::CreateVertexShader("Shaders/2DText_VS.cso", myVertexShader.GetAddressOf(), vsData);
	ISTE::DX11::CreatePixelShader("Shaders/2DText_PS.cso", myPixelShader.GetAddressOf());

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

bool ISTE::TextSystem::CreateBuffers()
{
	HRESULT result;
	//FrameBuffer init
	{
	D3D11_BUFFER_DESC objBufferDesc = {};
	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objBufferDesc.ByteWidth = sizeof(ObjBuffer);
	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myObjBuffer.GetAddressOf());
	if (FAILED(result))
		return false;
	}
	return true;
}

void ISTE::TextSystem::BindShader()
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->VSSetShader(myVertexShader.Get(), 0, 0);
	context->PSSetShader(myPixelShader.Get(), 0, 0);
	context->IASetInputLayout(myInputLayout.Get());
}

void ISTE::TextSystem::BindBuffers(TextRenderCommand& com)
{
	//ID3D11DeviceContext* context = myCtx->myDX11->GetContext();
	//
	//D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	//HRESULT res = context->Map(myObjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	//if (FAILED(res))
	//	return;
	//ObjBuffer* OBdata = (ObjBuffer*)(resource.pData);
	//
	//OBdata->myPosition = com.myposit;
	////OBdata->myColor = {};
	//OBdata->myScale = com.mySize;
	//OBdata->myUVEnd = com.myUvEnd;
	//OBdata->myUVStart = com.myUvStart;
	//OBdata->myRot = 0;
	//OBdata->myUVOffset= {0,0};
	//OBdata->myUVScale = { 1,1 };
	//OBdata->myColor = com.myColor; 
	//
	//context->Unmap(myObjBuffer.Get(), 0);
	//
	//
	//context->VSSetConstantBuffers(2, 1, myObjBuffer.GetAddressOf());
	//context->PSSetConstantBuffers(2, 1, myObjBuffer.GetAddressOf()); 
}
