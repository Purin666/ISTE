#pragma once
#include <wrl/client.h>
#include <dxgiformat.h>
#include <ISTE/Math/Vec4.h>
#include <ISTE/Math/Vec2.h>
using Microsoft::WRL::ComPtr;

struct ID3D11DepthStencilView; 
struct ID3D11ShaderResourceView; 
struct D3D11_VIEWPORT;

namespace ISTE
{
	struct Context;
	class DepthStencilTarget
	{
	public:
		DepthStencilTarget();
		~DepthStencilTarget();

		bool Create(unsigned int aWidth, unsigned int aHeight);
		bool Create(CU::Vec2Ui aRes);

		void PsBind(unsigned int aSlot);
		void VsBind(unsigned int aSlot);

		ID3D11DepthStencilView* GetDepthStencilRTV() { return myDSV.Get(); }
		ID3D11ShaderResourceView* GetDepthStencilSRV() { return mySRV.Get(); }

		void SetDepthTargetActive();
		void SetViewport();

		void Clear(float aDepth = 1, int aStencil = 0);

		unsigned int GetWidth() { return myWidth; }
		unsigned int GetHeight() { return myHeight; }

	private:
		Context* myCtx;

		ComPtr<ID3D11DepthStencilView> myDSV;
		ComPtr<ID3D11ShaderResourceView> mySRV;
		D3D11_VIEWPORT* myViewPort;

		unsigned int myWidth, myHeight;
	};
}

