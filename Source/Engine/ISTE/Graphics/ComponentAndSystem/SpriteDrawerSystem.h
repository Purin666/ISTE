#pragma once
#include <wrl/client.h>
#include <string>
#include <unordered_map>

#include <ISTE/ECSB/System.h>
#include <ISTE/Graphics/ComponentAndSystem/Sprite3DCommand.h>
#include <ISTE/Graphics/ComponentAndSystem/Sprite2DCommand.h>
#include <ISTE/Graphics/ComponentAndSystem/BillboardRenderCommand.h>
#include <ISTE/CU/MinHeap.hpp>
#include "ISTE/Graphics/RenderDefines.h"

using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11InputLayout;

namespace ISTE
{
	struct BillboardBatchValue {
		BatchedBillboardCommand*	myBatchedCommands;
		size_t						myBatchSize = 0; 
		TextureID					myTextureID;	//could just be the first/key value of the unordered_map  tbhtbh
	};
	
	enum class SpriteComplexity
	{
		e2DSprite,
		e3DSprite,
		eBillBoard,
		//eMore?
		eCount
	}; 


	struct Context;
	class GraphicsEngine;
	class SceneHandler;

	class SpriteDrawerSystem : public System
	{
	public: 
		~SpriteDrawerSystem();
		bool Init();
		void PrepareRenderCommands();

		void Draw();
		void DrawDepth();
		void ClearCommands();

		void Add2DSpriteRenderCommand(const Sprite2DRenderCommand&);
		void Add3DSpriteRenderCommand(const Sprite3DRenderCommand&);
		void AddBillboardCommand(const BillboardCommand&);
		void AddBatched2DSpriteRenderCommand(const Sprite2DRenderCommand&);
		void AddBatched3DSpriteRenderCommand(const Sprite3DRenderCommand&);
		void AddBatchedBillboardCommand(const BillboardCommand&);

		void Render2DSprites();
		void Render3DSprites();
		void RenderBillboards();
		void Render2DSpriteDepth();
		void Render3DSpriteDepth();
		void RenderBillboardDepth();

		void RenderBatched2DSprites();
		void RenderBatched3DSprites();
		void RenderBatchedBillboards();
		void RenderBatched2DSpriteDepth();
		void RenderBatched3DSpriteDepth();
		void RenderBatchedBillboardDepth();

	private: 

		void Prep3DSprites();
		void Prep2DSprites();
		void PrepBillboards();

		void PrepBatched3DSprites();
		void PrepBatched2DSprites();
		void PrepBatchedBillboards();
		Context* myCtx;
		SceneHandler* mySceneHandler;

		std::vector<Sprite3DRenderCommand>	my3DRenderCommands;
		std::vector<Sprite2DRenderCommand>	my2DRenderCommands;
		std::unordered_map<TextureID, BillboardBatchValue> myBatchedBillboardCommands;
		std::vector<BillboardCommand>	myBillboardCommands;

		ModelID mySpriteQuad;

	private: //shader stuff
		void BindSprite3DObjBuffer(const Sprite3DRenderCommand&);
		void BindSprite2DObjBuffer(const Sprite2DRenderCommand&); 
		void BindBillBoardBuffer(const BillboardCommand&);
		void BindShader(SpriteComplexity);

		bool InitShader(std::string aVSPath, std::string aPSPath, SpriteComplexity);
		bool Init2DSpriteBuffer();
		bool Init3DSpriteBuffer();
		bool InitBillboardBuffer();
		bool InitCommonGBuffer();

		ComPtr<ID3D11Buffer>		myCommonGBuffer; 
		ComPtr<ID3D11Buffer>		myObjBuffer[(int)SpriteComplexity::eCount];
		ComPtr<ID3D11InputLayout>	myInputLayout[(int)SpriteComplexity::eCount];
		ComPtr<ID3D11VertexShader>	myVertexShader[(int)SpriteComplexity::eCount];
		ComPtr<ID3D11PixelShader>	myPixelShader[(int)SpriteComplexity::eCount];
	};
}