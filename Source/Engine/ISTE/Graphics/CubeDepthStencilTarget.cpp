#include "CubeDepthStencilTarget.h"
#include <d3d11.h>
#include <ISTE/Context.h>
#include <ISTE/Graphics/DX11.h>

ISTE::CubeDepthStencilTarget::CubeDepthStencilTarget()
{
	myViewPort = nullptr;
}

ISTE::CubeDepthStencilTarget::~CubeDepthStencilTarget()
{
	if (myViewPort)
		delete myViewPort;
}

bool ISTE::CubeDepthStencilTarget::Create(unsigned int aWidth, unsigned int aHeight)
{
	myCtx = Context::Get();
	ID3D11Device* device = myCtx->myDX11->GetDevice();

	myViewPort = new D3D11_VIEWPORT;
	myViewPort->TopLeftX = 0;
	myViewPort->TopLeftY = 0;
	myViewPort->Width = (float)aWidth;
	myViewPort->Height = (float)aHeight;
	myViewPort->MinDepth = 0;
	myViewPort->MaxDepth = 1;

	//Depth Buffer
	ID3D11Texture2D* aDepthTexture;
	D3D11_TEXTURE2D_DESC depthStencilTextureDesc = { 0 };
	depthStencilTextureDesc.Width = static_cast<unsigned int>(aWidth);
	depthStencilTextureDesc.Height = static_cast<unsigned int>(aHeight);
	depthStencilTextureDesc.MipLevels = 1;
	depthStencilTextureDesc.ArraySize = 6;
	depthStencilTextureDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilTextureDesc.SampleDesc.Count = 1;
	depthStencilTextureDesc.SampleDesc.Quality = 0;
	depthStencilTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilTextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthStencilTextureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	HRESULT result = device->CreateTexture2D(&depthStencilTextureDesc, nullptr, &aDepthTexture);
	if (FAILED(result))
	{
		aDepthTexture->Release();
		return false;
	}


	D3D11_SHADER_RESOURCE_VIEW_DESC myDepthStencilDesc = {};
	myDepthStencilDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	myDepthStencilDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	myDepthStencilDesc.Texture2D.MipLevels = 1;
	myDepthStencilDesc.Texture2D.MostDetailedMip = 0;

	result = device->CreateShaderResourceView(aDepthTexture, &myDepthStencilDesc, mySRV.GetAddressOf());
	if (FAILED(result))
	{
		mySRV->Release();
		return false;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Flags = 0;
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; 
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	dsvDesc.Texture2DArray.MipSlice = 0;
	dsvDesc.Texture2DArray.ArraySize = 1;

	size_t sliceCount = 6;
	for (size_t i = 0; i < sliceCount; i++)
	{
		dsvDesc.Texture2DArray.FirstArraySlice = (unsigned int)i;
		result = device->CreateDepthStencilView(aDepthTexture, &dsvDesc, myDSV[i].GetAddressOf());
		if (FAILED(result))
		{
			myDSV[i]->Release();
			return false;
		}
	}


	aDepthTexture->Release();
	return true;
}

bool ISTE::CubeDepthStencilTarget::Create(CU::Vec2Ui aRes)
{
	return Create(aRes.x, aRes.y);
}

void ISTE::CubeDepthStencilTarget::PsBind(unsigned int aSlot)
{
	myCtx->myDX11->GetContext()->PSSetShaderResources(aSlot, 1, mySRV.GetAddressOf());
}

void ISTE::CubeDepthStencilTarget::VsBind(unsigned int aSlot)
{
	myCtx->myDX11->GetContext()->VSSetShaderResources(aSlot, 1, mySRV.GetAddressOf());
}

void ISTE::CubeDepthStencilTarget::SetDepthTargetActive(unsigned int aFace)
{
	ID3D11DeviceContext* d3dC = myCtx->myDX11->GetContext();
	ID3D11RenderTargetView* nullTarget[1] = { 0 };
	d3dC->OMSetRenderTargets(1, nullTarget, myDSV[aFace].Get());
	myCtx->myDX11->GetContext()->RSSetViewports(1, myViewPort);
}

void ISTE::CubeDepthStencilTarget::SetViewport()
{
	myCtx->myDX11->GetContext()->RSSetViewports(1, myViewPort);
}

void ISTE::CubeDepthStencilTarget::Clear(float aDepth, int aStencil)
{
	size_t sliceCount = 6;
	for (size_t i = 0; i < sliceCount; i++)
	{
		myCtx->myDX11->GetContext()->ClearDepthStencilView(myDSV[i].Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, aDepth, aStencil);
	}
}
