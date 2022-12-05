#pragma once
#include <wrl/client.h>
#include <dxgiformat.h>
#include <ISTE/Math/Vec4.h>
#include <ISTE/Math/Vec2.h>
using Microsoft::WRL::ComPtr;

struct ID3D11DepthStencilView;
struct ID3D11ShaderResourceView;
struct D3D11_VIEWPORT;
/*
Cube map order 

	+x 
	-x

	+y
	-y
	
	+z
	-z
*/


namespace ISTE
{
	struct Context;
	class CubeDepthStencilTarget
	{
	public:
		CubeDepthStencilTarget();
		~CubeDepthStencilTarget();

		bool Create(unsigned int aWidth, unsigned int aHeight);
		bool Create(CU::Vec2Ui aRes);

		void PsBind(unsigned int aSlot);
		void VsBind(unsigned int aSlot);

		ID3D11DepthStencilView* GetDepthStencil(unsigned int aSlice) { return myDSV[aSlice].Get(); }

		void SetDepthTargetActive(unsigned int aFace);
		void SetViewport();

		void Clear(float aDepth = 1, int aStencil = 0);

		unsigned int GetWidth() { return myWidth; }
		unsigned int GetHeight() { return myHeight; }

	private:
		Context* myCtx;

		ComPtr<ID3D11DepthStencilView> myDSV[6];
		ComPtr<ID3D11ShaderResourceView> mySRV;
		D3D11_VIEWPORT* myViewPort;

		unsigned int myWidth, myHeight;
	};
}

