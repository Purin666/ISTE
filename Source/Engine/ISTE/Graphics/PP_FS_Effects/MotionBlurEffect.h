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
	class MotionBlurEffect
	{
	public:
		struct MotionBlurData { 
			float			mySamples		= 10;
			float			myBlurStrength	= 0.3f;
			CU::Vec2f Garb;
		};
	public:
		MotionBlurEffect();
		~MotionBlurEffect();
		bool Init();
		bool Draw();

		void SetEnabled(bool aFlag) { myEnabledFlag = aFlag; };

		MotionBlurData& GetBufferData() { return myMotionBlurBufferData; } 

	private: 
		friend class DX11;
		Context*				myCtx;
		ComPtr<ID3D11Buffer>    myMotionBlurBuffer;
		MotionBlurData          myMotionBlurBufferData; 
		bool					myEnabledFlag = false;
		
		void BindBuffers();
		bool InitBuffers();
	};
}