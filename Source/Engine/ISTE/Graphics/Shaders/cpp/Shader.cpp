#include "Shader.h" 
#include <d3d11.h>
#include <fstream>

#include "ISTE/Context.h"
#include "ISTE/Graphics/DX11.h"

ISTE::Shader::Shader()
{
	myCtx = nullptr;
}

ISTE::Shader::~Shader()
{
}

bool ISTE::Shader::Init(std::wstring aVSPath, std::wstring aPSPath)
{
	myCtx = Context::Get();
	ID3D11Device* aDevice = myCtx->myDX11->GetDevice();

	HRESULT result;
	std::string vsData;
	{
		std::ifstream vsFile;
		vsFile.open(aVSPath, std::ios::binary);
		vsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
		vsFile.close();
		result = aDevice->CreateVertexShader(vsData.data(), vsData.size(), nullptr, myVertexShader.GetAddressOf());
		if (FAILED(result))
			return false;
		std::ifstream psFile;
		psFile.open(aPSPath, std::ios::binary);
		std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
		psFile.close();

		HRESULT result = aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, myPixelShader.GetAddressOf());
		if (FAILED(result))
			return false;
	}

	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"UV",
				0,
				DXGI_FORMAT_R32G32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"NORMAL",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"BINORMAL",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"TANGENT",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"BONES",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"WEIGHTS",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},

		};

		int elementCount = sizeof(layout) / sizeof(layout[0]);
		result = aDevice->CreateInputLayout(layout, elementCount, vsData.data(), vsData.size(), myInputLayout.GetAddressOf());
		if (FAILED(result))
			return false;
	}

	if (!CreateBuffers())
		return false;
	return true; 
}

void ISTE::Shader::BindShader()
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	context->VSSetShader(myVertexShader.Get(), 0, 0);
	context->PSSetShader(myPixelShader.Get(), 0, 0);
	context->IASetInputLayout(myInputLayout.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}
