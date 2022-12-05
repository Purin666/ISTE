#pragma once
#include <wrl/client.h>
#include "ISTE/Math/Vec2.h"
#include "ISTE/Math/Vec4.h"



using Microsoft::WRL::ComPtr;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct D3D11_VIEWPORT;

namespace ISTE
{
	struct Context;

	enum class GBufferTexture {
		eWorldPosition,
		eAlbedo,
		ePixelNormal,
		eVertexNormal,
		eMaterial,
		eCustomData,
		eEntityData,
		eCount,
	};
	class GBuffer
	{	
	public:
		GBuffer();
		~GBuffer();

		bool Create(CU::Vec2Ui aSize);
		void SetAsActiveTarget(ID3D11DepthStencilView* aDepthBuffer = nullptr);
		void SetAsResourceOnSlot(GBufferTexture, unsigned int); //psset
		void PSSetAllResources(unsigned int, bool aExcludeEntFlag = true);
		inline ID3D11ShaderResourceView* GetSRV(GBufferTexture aTextureType) { return mySRVs[(int)aTextureType].Get(); }
		inline ID3D11RenderTargetView* GetRTV(GBufferTexture aTextureType) { return myRTVs[(int)aTextureType].Get(); }
		void ClearTextures(CU::Vec4f aClearColor = {0,0,0,0});

	private:
		Context* myCtx;
		ComPtr<ID3D11Texture2D>				myTextures[(int)GBufferTexture::eCount];
		ComPtr<ID3D11ShaderResourceView>	mySRVs[(int)GBufferTexture::eCount];
		ComPtr<ID3D11RenderTargetView>		myRTVs[(int)GBufferTexture::eCount];

		ComPtr<ID3D11Texture2D>				myOutTexture;
		ComPtr<ID3D11RenderTargetView>		myOutRTV;

		D3D11_VIEWPORT*						myViewport;
	};
};