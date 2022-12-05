#include "RadialBlurEffect.h"
#include "SSAOEffect.h"

#include <d3d11.h>
#include <dxgiformat.h>

#include "ISTE/Context.h"
#include "ISTE/Graphics/DX11.h"
#include "ISTE/CU/UtilityFunctions.hpp"
#include "ISTE/Graphics/Resources/ShaderManager.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/RenderStateManager.h"
#include "ISTE/WindowsWindow.h"

ISTE::RadialBlurEffect::RadialBlurEffect()
{
}

ISTE::RadialBlurEffect::~RadialBlurEffect()
{
}

bool ISTE::RadialBlurEffect::Init()
{
    myCtx = Context::Get();

    if (!InitBuffers())
        return false;
   // if (!myMotionBlurTarget.Create(myCtx->myWindow->GetResolution(), DXGI_FORMAT_R32G32B32A32_FLOAT))
   //     return false;

    return true;
}

bool ISTE::RadialBlurEffect::Draw()
{
    if (!myEnabledFlag)
    {
        myCtx->myShaderManager->GetFullscreenEffect(eFullscreenEffects::ePPCopy).Render();
        return false;
    }
    BindBuffers();
    myCtx->myShaderManager->GetFullscreenEffect(eFullscreenEffects::ePPRadialBlur).Render();
    return true;
}

void ISTE::RadialBlurEffect::BindBuffers()
{
    D3D11_MAPPED_SUBRESOURCE resource;
    ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

    HRESULT res = context->Map(myRadialBlurBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    if (FAILED(res))
        return;
    RadialBlurData* OBdata = (RadialBlurData*)(resource.pData);
    *OBdata = myRadialBlurBufferData;
    context->Unmap(myRadialBlurBuffer.Get(), 0);

    context->PSSetConstantBuffers(2, 1, myRadialBlurBuffer.GetAddressOf());
}

bool ISTE::RadialBlurEffect::InitBuffers()
{
    D3D11_BUFFER_DESC desc = {};
    desc.Usage          = D3D11_USAGE_DYNAMIC;
    desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.ByteWidth      = sizeof(RadialBlurData);

    HRESULT res = myCtx->myDX11->GetDevice()->CreateBuffer(&desc, 0, myRadialBlurBuffer.GetAddressOf());
    if (FAILED(res))
        return false;

    return true;
}
