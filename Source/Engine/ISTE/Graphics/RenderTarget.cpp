#include "RenderTarget.h"
#include <d3d11.h>

#include <ISTE/Context.h>
#include <ISTE/Engine.h>

#include "ISTE/Logger/Logger.h"

#include <iostream>



ISTE::RenderTarget::RenderTarget()
{
    myWidth = 0;
    myHeight = 0;
    myViewPort = nullptr;
}

ISTE::RenderTarget::~RenderTarget()
{
    if (myViewPort)
        delete myViewPort;
}


bool ISTE::RenderTarget::Create(CU::Vec2Ui aRes, DXGI_FORMAT aFormat)
{
    return Create(aRes.x, aRes.y, aFormat);
}

bool ISTE::RenderTarget::Create(unsigned int aWidth, unsigned int aHeight, DXGI_FORMAT aFormat)
{
    myWidth = aWidth;
    myHeight = aHeight;

    myViewPort = new D3D11_VIEWPORT;
    myViewPort->TopLeftX = 0;
    myViewPort->TopLeftY = 0;
    myViewPort->Width = (float)aWidth;
    myViewPort->Height = (float)aHeight;
    myViewPort->MinDepth = 0;
    myViewPort->MaxDepth = 1;

    myCtx = Context::Get();

    D3D11_TEXTURE2D_DESC texDesc = { 0 };
    texDesc.Width = aWidth;
    texDesc.Height = aHeight;
    texDesc.Format = aFormat;
    texDesc.MipLevels = 1;
    texDesc.ArraySize = 1;
    texDesc.SampleDesc.Count = 1;
    texDesc.SampleDesc.Quality = 0;
    texDesc.Usage = D3D11_USAGE_DEFAULT;
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
    texDesc.CPUAccessFlags = 0;
    texDesc.MiscFlags = 0;


    ID3D11Texture2D* texture = nullptr; 

    HRESULT result;
    result = myCtx->myDX11->GetDevice()->CreateTexture2D(&texDesc, 0, &texture);
    if (FAILED(result))
    {
        texture->Release();
#ifdef _DEBUG 
        std::cout << "Failed to create renderTarget texture." << std::endl;
#endif
        return false;
    }

    result = myCtx->myDX11->GetDevice()->CreateShaderResourceView(texture, 0, mySRV.GetAddressOf());
    if (FAILED(result))
    {
        texture->Release();
#ifdef _DEBUG 
        std::cout << "Failed to create renderTarget SRV." << std::endl; 
#endif
        return false;
    }

    result = myCtx->myDX11->GetDevice()->CreateRenderTargetView(texture, 0, myRTV.GetAddressOf());
    if (FAILED(result))
    {
        texture->Release();
#ifdef _DEBUG
        std::cout << "Failed to create renderTarget RTV." << std::endl; 
#endif 
        return false;
    }
    texture->Release();
    return true;
}
    
bool ISTE::RenderTarget::Create(ID3D11Texture2D* aTexture)
{ 
    myCtx = Context::Get();
    
    {//gets height and width and stores it.
    D3D11_TEXTURE2D_DESC texDesc = { 0 };
    aTexture->GetDesc(&texDesc);

    myWidth = texDesc.Width;
    myHeight = texDesc.Height;

    myViewPort = new D3D11_VIEWPORT;
    myViewPort->TopLeftX = 0;
    myViewPort->TopLeftY = 0;
    myViewPort->Width = (float)myWidth;
    myViewPort->Height = (float)myHeight;
    myViewPort->MinDepth = 0;
    myViewPort->MaxDepth = 1;

    } 

    HRESULT result;  
    result = myCtx->myDX11->GetDevice()->CreateRenderTargetView(aTexture, 0, myRTV.GetAddressOf());
    if (FAILED(result))
    {
#ifdef _DEBUG
        std::cout << "Failed to create renderTarget RTV." << std::endl;
        Logger::DebugOutputError(L"Failed to create renderTarget RTV.");
#endif  
        return false;
    }

    return true;
}
    
void ISTE::RenderTarget::PsBind(unsigned int aSlot) 
{ 
    myCtx->myDX11->GetContext()->PSSetShaderResources(aSlot, 1, mySRV.GetAddressOf());
}
    
void ISTE::RenderTarget::VsBind(unsigned int aSlot)
{  
    myCtx->myDX11->GetContext()->VSSetShaderResources(aSlot, 1, mySRV.GetAddressOf());
}
    
void ISTE::RenderTarget::SetActiveTarget(ID3D11DepthStencilView* aDepthBuffer)
{ 
    ID3D11DeviceContext* aContext = myCtx->myDX11->GetContext();
     
    aContext->OMSetRenderTargets(1, myRTV.GetAddressOf(), aDepthBuffer);
    aContext->RSSetViewports(1, myViewPort);
}
    
void ISTE::RenderTarget::SetViewPort()
{
    myCtx->myDX11->GetContext()->RSSetViewports(1, myViewPort);
}
    
void ISTE::RenderTarget::Clear(const CU::Vec4f& color)
{
    myCtx->myDX11->GetContext()->ClearRenderTargetView(myRTV.Get(), &color.x);
}