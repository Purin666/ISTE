#pragma once
#include <wrl/client.h> 

#include <vector>
#include <string>

#include <ISTE/Graphics/ComponentAndSystem/DecalRenderCommand.h>
#include <ISTE/ECSB/System.h> 
#include <ISTE/Graphics/RenderDefines.h>
#include <ISTE/CU/MinHeap.hpp>


using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11InputLayout;

namespace ISTE
{
	struct Context;
	class DecalDrawerSystem : public System
	{
	public:
		DecalDrawerSystem()  = default;
		~DecalDrawerSystem() = default;
		bool Init();
		void PrepareRenderCommands();
		void AddDecalRenderCommand(const DecalRenderCommand&);
		void Draw();
		void DrawDebugLines();
		void ClearRenderCommands();


	private:
		Context* myCtx;
		CU::MinHeap<DecalRenderCommand> myDecalRenderCommands;
		ModelID myCubeID;

	private: //shader stuff
		bool InitShader(std::string aVSPath, std::string aPSPath);
		bool InitDecalObjBuffer();

		void BindShader();
		void BindObjBuffer(const DecalRenderCommand*);


		ComPtr<ID3D11Buffer>		myDecalPSObjBuffer;
		ComPtr<ID3D11Buffer>		myDecalVSObjBuffer;
		ComPtr<ID3D11VertexShader>	myDecalVertexShader;
		ComPtr<ID3D11PixelShader>	myDecalPixelShader;
		ComPtr<ID3D11PixelShader>	myColorPixelShader;
		ComPtr<ID3D11InputLayout>	myInputLayout;

	};
};