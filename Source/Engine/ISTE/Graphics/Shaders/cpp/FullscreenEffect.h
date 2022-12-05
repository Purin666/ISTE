#pragma once
#include <wrl/client.h>
#include <string>

using Microsoft::WRL::ComPtr;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

namespace ISTE
{
	struct Context;

	class FullscreenEffect
	{
	public:
		FullscreenEffect();
		~FullscreenEffect();
		bool Init(std::string aPSPath);
		void Render();

	private:
		Context* myCtx;

		ComPtr<ID3D11VertexShader> myVertexShader;
		ComPtr<ID3D11PixelShader> myPixelShader;

	};

}