#include "MotionBlurEffect.h"
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

ISTE::MotionBlurEffect::MotionBlurEffect()
{
}

ISTE::MotionBlurEffect::~MotionBlurEffect()
{
}

bool ISTE::MotionBlurEffect::Init()
{
    myCtx = Context::Get();

    if (!InitBuffers())
        return false;
   // if (!myMotionBlurTarget.Create(myCtx->myWindow->GetResolution(), DXGI_FORMAT_R32G32B32A32_FLOAT))
   //     return false;

    return true;
}

bool ISTE::MotionBlurEffect::Draw()
{
    if (!myEnabledFlag)
    {
        myCtx->myShaderManager->GetFullscreenEffect(eFullscreenEffects::ePPCopy).Render();
        return false;
    }
    BindBuffers();
    myCtx->myShaderManager->GetFullscreenEffect(eFullscreenEffects::ePPMotionBlur).Render();
    return true;
}

void ISTE::MotionBlurEffect::BindBuffers()
{
    D3D11_MAPPED_SUBRESOURCE resource;
    ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

    HRESULT res = context->Map(myMotionBlurBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    if (FAILED(res))
        return;
    MotionBlurData* OBdata = (MotionBlurData*)(resource.pData);
    *OBdata = myMotionBlurBufferData;
    context->Unmap(myMotionBlurBuffer.Get(), 0);

    context->PSSetConstantBuffers(2, 1, myMotionBlurBuffer.GetAddressOf());
}

bool ISTE::MotionBlurEffect::InitBuffers()
{
    D3D11_BUFFER_DESC desc = {};
    desc.Usage          = D3D11_USAGE_DYNAMIC;
    desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.ByteWidth      = sizeof(MotionBlurData);

    HRESULT res = myCtx->myDX11->GetDevice()->CreateBuffer(&desc, 0, myMotionBlurBuffer.GetAddressOf());
    if (FAILED(res))
        return false;

    return true;
}
