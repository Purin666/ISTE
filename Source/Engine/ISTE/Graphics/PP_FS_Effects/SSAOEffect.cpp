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
  

//struct GaussianBlurBufferData {
//
//    //Reserch this shit...
//
//
//
//};

bool ISTE::SSAOEffect::Init()
{
    myCtx = Context::Get();

    if (!InitBuffers())
        return false; 
    if (!mySSAOTarget.Create(myCtx->myWindow->GetResolution(), DXGI_FORMAT_R32G32B32A32_FLOAT))
        return false;
    mySSAODepthTarget.Create(myCtx->myWindow->GetResolution());

    GenerateKernals();

    return true;
}

void ISTE::SSAOEffect::Draw()
{ 
    mySSAOTarget.Clear();
    mySSAODepthTarget.Clear();

    mySSAODepthTarget.SetDepthTargetActive();
    myCtx->myRenderStateManager->SetRasterState(RasterizerState::eBiasedRaster);
    myCtx->myRenderStateManager->SetSampleState(AdressMode::eClamp,SamplerState::ePoint, 2);
    myCtx->myGraphicsEngine->RenderDepth();

    mySSAOTarget.SetActiveTarget();
    myCtx->myRenderStateManager->SetRasterState(RasterizerState::eBackFaceCulling);
    mySSAODepthTarget.PsBind(0);
    myCtx->myDX11->GetMainGBuffer().SetAsResourceOnSlot(GBufferTexture::ePixelNormal, 4 + (int)GBufferTexture::ePixelNormal);
    myCtx->myDX11->GetMainGBuffer().SetAsResourceOnSlot(GBufferTexture::eWorldPosition, 4 + (int)GBufferTexture::eWorldPosition);
    BindBuffers();
    myCtx->myShaderManager->GetFullscreenEffect(eFullscreenEffects::ePPSSAO).Render(); 

    ID3D11ShaderResourceView* nullSRV[1] = { nullptr };
    myCtx->myDX11->GetContext()->PSSetShaderResources(0, 1, nullSRV);
} 

void ISTE::SSAOEffect::GenerateKernals()
{
    size_t i = 0;
    CU::Vec3f intermediateVec3;
    for (; i < MAX_SSAO_KERNAL_SIZE; i++)
    {
        intermediateVec3 = {
            CU::GetRandomFloat(-1,1),
            CU::GetRandomFloat(-1,1),
            CU::GetRandomFloat(0,1)
        }; 
        intermediateVec3.Normalize();

        float scale = float(i) / float(MAX_SSAO_KERNAL_SIZE);
        scale = CU::Lerp(0.1f, 1.0f, scale * scale);
        intermediateVec3 *= scale;

        mySSAOBufferData.myKernals[i] = {
            intermediateVec3.x,
            intermediateVec3.y,
            intermediateVec3.z,
            0
        };
    }
}

void ISTE::SSAOEffect::BindBuffers()
{
    D3D11_MAPPED_SUBRESOURCE resource;
    ID3D11DeviceContext* context = myCtx->myDX11->GetContext();
    
    HRESULT res = context->Map(mySSAOBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    if (FAILED(res))
    	return;
    SSAOBufferData* OBdata = (SSAOBufferData*)(resource.pData);
    mySSAOBufferData.myWorldToViewINV = CU::Matrix4x4f::Inverse(myCtx->myGraphicsEngine->GetCamera().GetWorldToClip());
    *OBdata = mySSAOBufferData; 
    context->Unmap(mySSAOBuffer.Get(), 0);
    
    context->PSSetConstantBuffers(2, 1, mySSAOBuffer.GetAddressOf());
}

bool ISTE::SSAOEffect::InitBuffers()
{
    D3D11_BUFFER_DESC desc = {};
    desc.Usage          = D3D11_USAGE_DYNAMIC;
    desc.BindFlags      = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.ByteWidth      = sizeof(SSAOBufferData);

    HRESULT res = myCtx->myDX11->GetDevice()->CreateBuffer(&desc, 0, mySSAOBuffer.GetAddressOf()); 
    if (FAILED(res))
        return false; 
    return true;
}
 