#pragma once
#include <wrl/client.h>
#include <dxgiformat.h>
#include <ISTE/Math/Vec4.h>
#include <ISTE/Math/Vec2.h>
using Microsoft::WRL::ComPtr;

struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

struct D3D11_VIEWPORT;
namespace ISTE {

	struct Context;
	class RenderTarget
	{
	public:
		RenderTarget();
		~RenderTarget();

		bool Create(CU::Vec2Ui, DXGI_FORMAT);
		bool Create(unsigned int aWidth, unsigned int aHeight, DXGI_FORMAT);
		bool Create(ID3D11Texture2D*);

		void PsBind(unsigned int aSlot);
		void VsBind(unsigned int aSlot);
		void SetActiveTarget(ID3D11DepthStencilView* = nullptr);

		void SetViewPort();
		D3D11_VIEWPORT*& GetViewPort() { return myViewPort; }

		ID3D11ShaderResourceView* GetSRV() { return mySRV.Get(); }
		ID3D11RenderTargetView* GetRTV() { return myRTV.Get(); }

		void Clear(const CU::Vec4f& color = { 0,0,0,1 });

		unsigned int GetWidth() { return myWidth; }
		unsigned int GetHeight() { return myHeight; }

	private:
		ComPtr<ID3D11ShaderResourceView> mySRV;
		ComPtr<ID3D11RenderTargetView> myRTV;
		D3D11_VIEWPORT* myViewPort;

		Context* myCtx; 
		unsigned int myWidth, myHeight;
	};
};