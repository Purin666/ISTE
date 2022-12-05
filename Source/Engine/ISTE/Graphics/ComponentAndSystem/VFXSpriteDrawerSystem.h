#pragma once
#include <wrl/client.h>
#include <string>

#include <ISTE/ECSB/System.h>
#include <ISTE/Graphics/ComponentAndSystem/VFXSprite3DCommand.h>
#include <ISTE/Graphics/ComponentAndSystem/VFXSprite2DCommand.h>
#include <ISTE/CU/MinHeap.hpp>
#include "ISTE/Graphics/RenderDefines.h"

using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11InputLayout;

namespace ISTE
{
	enum class VFXSpriteComplexity
	{
		eVFX2DSprite,
		eVFX3DSprite,
		//eBillBoard
		//eMore?
		eCount
	}; 


	struct Context;
	class GraphicsEngine;
	class SceneHandler;

	class VFXSpriteDrawerSystem : public System
	{
	public: 
		~VFXSpriteDrawerSystem();
		bool Init();
		void PrepareRenderCommands();

		void Draw();
		void Render2DSprites();
		void Render3DSprites();
		void DrawDepth();
		void ClearCommands();
		void Add2DSpriteRenderCommand(VFXSprite2DRenderCommand);
		void Add3DSpriteRenderCommand(VFXSprite3DRenderCommand); 

	private: 
		void Prep3DSprites();
		void Prep2DSprites(); 

		Context* myCtx;
		SceneHandler* mySceneHandler;

		std::vector<VFXSprite3DRenderCommand> my3DRenderCommands;
		std::vector<VFXSprite2DRenderCommand> my2DRenderCommands;

		ModelID mySpriteQuad;

	private: //shader stuff
		void BindSprite3DObjBuffer(const VFXSprite3DRenderCommand&);
		void BindSprite2DObjBuffer(const VFXSprite2DRenderCommand&); 
		void BindShader(VFXSpriteComplexity);

		bool InitShader(std::string aVSPath, std::string aPSPath, VFXSpriteComplexity);
		bool Init2DSpriteBuffer();
		bool Init3DSpriteBuffer();

		ComPtr<ID3D11Buffer>		myObjBuffer[(int)VFXSpriteComplexity::eCount];
		ComPtr<ID3D11InputLayout>	myInputLayout[(int)VFXSpriteComplexity::eCount];
		ComPtr<ID3D11VertexShader>	myVertexShader[(int)VFXSpriteComplexity::eCount];
		ComPtr<ID3D11PixelShader>	myPixelShader[(int)VFXSpriteComplexity::eCount];
	};
}