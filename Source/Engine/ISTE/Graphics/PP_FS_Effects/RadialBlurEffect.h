#pragma once
#include <wrl/client.h>  
#include "ISTE/Graphics/RenderTarget.h"
#include "ISTE/Graphics/DepthStencilTarget.h"
#include "ISTE/Graphics/RenderDefines.h"
#include "ISTE/Math/Vec4.h"
#include "ISTE/Math/Matrix4x4.h"

using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11InputLayout;

namespace ISTE
{
	class RadialBlurEffect
	{
	public:
		struct RadialBlurData { 
			float			myBlurStrength	= 0.5f;
			float			mySamples		= 10;
			CU::Vec2f Garb;
		};
	public:
		RadialBlurEffect();
		~RadialBlurEffect();
		bool Init();
		bool Draw();

		void SetEnabled(bool aFlag) { myEnabledFlag = aFlag; };

		RadialBlurData& GetBufferData() { return myRadialBlurBufferData; }

	private: 
		friend class DX11;
		Context*				myCtx;
		ComPtr<ID3D11Buffer>    myRadialBlurBuffer;
		RadialBlurData          myRadialBlurBufferData;
		bool					myEnabledFlag = false;
		
		void BindBuffers();
		bool InitBuffers();
	};
}