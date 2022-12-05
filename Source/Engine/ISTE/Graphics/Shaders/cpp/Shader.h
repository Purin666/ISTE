 #pragma once
#include "ISTE/ECSB/ECSDefines.hpp"

#include <wrl/client.h>
#include <string> 

using Microsoft::WRL::ComPtr;
struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11InputLayout;

namespace ISTE
{
	struct Context;
	class Shader
	{
	public:
		Shader();
		virtual ~Shader();

		bool Init(std::wstring aVSPath, std::wstring aPSPath);
		virtual bool CreateBuffers() { return true; };
		
		

		virtual void Render(EntityID /*aEntity*/) {};
		virtual void BindShader();
	protected:
		Context* myCtx;

		ComPtr<ID3D11InputLayout> myInputLayout;
		ComPtr<ID3D11VertexShader> myVertexShader;
		ComPtr<ID3D11PixelShader> myPixelShader;

	};
}