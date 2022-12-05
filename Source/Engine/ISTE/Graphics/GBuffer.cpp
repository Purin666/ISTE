#include "GBuffer.h"
#include <d3d11.h>
#include <cassert>

#include "ISTE/Context.h"
#include "ISTE/Graphics/DX11.h"
#include "ISTE/Engine.h"



ISTE::GBuffer::GBuffer()
{
	myViewport	= nullptr;
	myCtx		= nullptr;
}

ISTE::GBuffer::~GBuffer()
{
	if(myViewport)
		delete myViewport;
}

bool ISTE::GBuffer::Create(CU::Vec2Ui aSize)
{
	myCtx = Context::Get();
	ID3D11Device* device = myCtx->myDX11->GetDevice();
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();


	HRESULT result;
	DXGI_FORMAT textureFormats[(int)GBufferTexture::eCount] =
	{
	DXGI_FORMAT_R32G32B32A32_FLOAT,	// Position
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,// Albedo
	DXGI_FORMAT_R10G10B10A2_UNORM,	// PixelNormal, 
	DXGI_FORMAT_R10G10B10A2_UNORM,	// VertexNormal, 
	DXGI_FORMAT_R8G8B8A8_UNORM,		// Material
	DXGI_FORMAT_R32G32B32A32_FLOAT,	// AO, Depth flags
	DXGI_FORMAT_R32G32B32A32_FLOAT,	// Id, Velocity
	};  

	D3D11_TEXTURE2D_DESC desc = { 0 };
	desc.Width = aSize.x;
	desc.Height = aSize.y;
	desc.MipLevels = 1;
	desc.ArraySize = 1;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	for (unsigned int idx = 0; idx < (int)GBufferTexture::eCount; idx++)
	{
		desc.Format = textureFormats[idx];

		result = device->CreateTexture2D(&desc, nullptr, myTextures[idx].GetAddressOf());
		assert(SUCCEEDED(result));
		
		result = device->CreateRenderTargetView(
			myTextures[idx].Get(),
			nullptr,
			myRTVs[idx].GetAddressOf());
		assert(SUCCEEDED(result));

		result = device->CreateShaderResourceView(
			myTextures[idx].Get(),
			nullptr,
			mySRVs[idx].GetAddressOf());
		assert(SUCCEEDED(result));
	}

	myViewport = new D3D11_VIEWPORT(
		D3D11_VIEWPORT{
		0,
		0,
		static_cast<float>(desc.Width),
		static_cast<float>(desc.Height),
		0,
		1
		});
	return true;
}

void ISTE::GBuffer::SetAsActiveTarget(ID3D11DepthStencilView* aDepthBuffer)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext(); 
	context->OMSetRenderTargets((int)GBufferTexture::eCount, myRTVs[0].GetAddressOf(), aDepthBuffer);
	context->RSSetViewports(1, myViewport);
}

void ISTE::GBuffer::SetAsResourceOnSlot(GBufferTexture aTexture, unsigned int aSlot)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	context->PSSetShaderResources(aSlot, 1, mySRVs[(int)aTexture].GetAddressOf());
	context->RSSetViewports(1, myViewport);
}

void ISTE::GBuffer::PSSetAllResources(unsigned int aSlot, bool aExcludeEntFlag)
{ 
	//we dont want to set the out resource to the gpu
	//because that data is only reserved for gpu-cpu communication and should not be used by anything else.
	//there is probably a nicer way of doing this.

	ID3D11DeviceContext* context = myCtx->myDX11->GetContext(); 
	context->PSSetShaderResources(aSlot, (int)GBufferTexture::eCount - (int)aExcludeEntFlag, mySRVs[0].GetAddressOf());
	context->RSSetViewports(1, myViewport);
}
 

void ISTE::GBuffer::ClearTextures(CU::Vec4f aClearColor)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();
	for (size_t i = 0; i < (int)GBufferTexture::eCount; i++)
	{
		context->ClearRenderTargetView(myRTVs[i].Get(), &aClearColor.x);
	}
}
