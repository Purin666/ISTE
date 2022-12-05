#pragma once
#include "../Math/Vec4.h"
#include "../Math/Vec2.h"

#include <wrl/client.h>   
#include <string>

#include "RenderTarget.h"
#include "DepthStencilTarget.h"
#include "RenderTarget.h"
#include "DepthStencilTarget.h"
#include "GBuffer.h"

using Microsoft::WRL::ComPtr;
struct ID3D11Device;
struct ID3D11DeviceContext;
struct IDXGISwapChain;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11RenderTargetView;
struct D3D11_VIEWPORT;

struct ID3D11PixelShader;
struct ID3D11VertexShader;

namespace ISTE {
	class DX11
	{
	public: //static helper functions

		/*
		*	@param aVSPath: The path to a vertex shader.
		*	@param aVsAdRef: A pointer to the adress of the soon to be created Vertex Shader.
		*	@param outVsData: The out Vertex Shader data. Outed to be used in the InputLayout
		*	@return returns a bool success flag.
		*/
		static bool CreateVertexShader(const std::string& aVSPath, ID3D11VertexShader** aVsAdRef, std::string& someOutVsData);

		/*
		*	@param aPSPath: The path to a vertex shader.
		*	@param aPsAdRef: A pointer to the adress of the soon to be created Vertex Shader.
		*	@return returns a bool success flag.
		*/
		static bool CreatePixelShader(const std::string& aPSPath, ID3D11PixelShader** aPsAdRef);
		
	public:
		DX11();
		~DX11();

		bool Init(const HWND aWindowsHandle);

		void BeginFrame();
		void EndFrame();

		ID3D11DeviceContext* GetContext() { return myContext.Get(); }
		ID3D11DeviceContext* GetNewThreadContext();
		void EndThreadContext(ID3D11DeviceContext* aDeferredContext);
		ID3D11Device* GetDevice() { return myDevice.Get(); }

		GBuffer& GetMainGBuffer() { return myGBuffer; };
		GBuffer& GetIntermediateGBuffer() {return myIntermediateGBuffer; };

		RenderTarget* GetBackBuffer() { return &myBackBuffer; }
		DepthStencilTarget* GetDepthBuffer() { return &myDepthBuffer; }

		void SetSceneAndEntityTextureActive(ID3D11DepthStencilView* = nullptr);

		void SetViewPortActive(); 

		RenderTarget& GetSceneTarget() { return mySceneTexture; }
		RenderTarget& GetIntermediateTarget() { return myIntermediateTexture; }

		void SetResolution(CU::Vec2Ui aNewRes); 
		CU::Vec2Ui GetBackBufferRes();

		void SetVoidColor(CU::Vec4f aVoidColor) { myVoidColor = aVoidColor; }
		CU::Vec4f GetVoidColor() { return myVoidColor; }

	private:
		friend class WindowsWindow;
		friend class GraphicsEngine;
		CU::Vec4f myVoidColor = { 0,0,0,1 };
		CU::Vec2Ui myRes;

		RenderTarget myBackBuffer;
		RenderTarget myDownAndUpSampleTextures[5];
		RenderTarget myIntermediateTexture;
		RenderTarget mySceneTexture; 

		GBuffer myGBuffer;
		GBuffer myIntermediateGBuffer;
		
		DepthStencilTarget myDepthBuffer;
		ComPtr<ID3D11Device> myDevice;
		ComPtr<ID3D11DeviceContext> myContext;
		ComPtr<IDXGISwapChain> mySwapChain;
		D3D11_VIEWPORT* myViewPort;
	};
};