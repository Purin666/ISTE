#include "RenderStateManager.h"

//libs
#include <d3d11.h>

//Engine
#include <ISTE/Context.h>
#include <ISTE/GRaphics/DX11.h>
#include <ISTE/Context.h>

ISTE::RenderStateManager::RenderStateManager()
{
}

ISTE::RenderStateManager::~RenderStateManager()
{
}

bool ISTE::RenderStateManager::Init()
{
    myCtx = Context::Get();

    if (!CreateSamplerStates())
        return false;
    if (!CreateRasterStates())
        return false;
    if (!CreateDepthStates())
        return false;
    if (!CreateBlendStates())
        return false;

    return true;
}

void ISTE::RenderStateManager::SetSampleState(AdressMode aAM,SamplerState aSS, unsigned int aSlot)
{ 
    //if sampler is needed in vs just make another func
    myCtx->myDX11->GetContext()->PSSetSamplers(aSlot, 1, mySamplerStates[(int)aAM][(int)aSS].GetAddressOf());
}

void ISTE::RenderStateManager::SetDepthState(DepthState aState, ReadWriteState aRoWState)
{
    myCtx->myDX11->GetContext()->OMSetDepthStencilState(myDepthStencilStates[(int)aState][(int)aRoWState].Get(), 0);
}

void ISTE::RenderStateManager::SetRasterState(RasterizerState aState)
{
    myCtx->myDX11->GetContext()->RSSetState(myRasterizerStates[(int)aState].Get()); 
}

void ISTE::RenderStateManager::SetBlendState(BlendState aState)
{
    float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
    UINT sampleMask = 0xffffffff;
    myCtx->myDX11->GetContext()->OMSetBlendState(myBlendStates[(int)aState].Get(), blendFactor, sampleMask);
}

bool ISTE::RenderStateManager::CreateSamplerStates()
{
    ID3D11Device* device = myCtx->myDX11->GetDevice();
    HRESULT result;

#pragma region WRAP
    {
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        result = device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)AdressMode::eWrap][(int)SamplerState::eTriLinear].GetAddressOf());
        if (FAILED(result))
            return false;
    }
    {
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        result = device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)AdressMode::eWrap][(int)SamplerState::eBiliniear].GetAddressOf());
        if (FAILED(result))
            return false;
    }
    {
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        result = device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)AdressMode::eWrap][(int)SamplerState::eAnisotropic].GetAddressOf());
        if (FAILED(result))
            return false;
    }
    {
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        result = device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)AdressMode::eWrap][(int)SamplerState::ePoint].GetAddressOf());
        if (FAILED(result))
            return false;
    }
#pragma endregion

#pragma region CLAMP
    {
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        result = device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)AdressMode::eClamp][(int)SamplerState::eTriLinear].GetAddressOf());
        if (FAILED(result))
            return false;
    }
    {
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        result = device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)AdressMode::eClamp][(int)SamplerState::eBiliniear].GetAddressOf());
        if (FAILED(result))
            return false;
    }
    {
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        result = device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)AdressMode::eClamp][(int)SamplerState::eAnisotropic].GetAddressOf());
        if (FAILED(result))
            return false;
    }
    {
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        result = device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)AdressMode::eClamp][(int)SamplerState::ePoint].GetAddressOf());
        if (FAILED(result))
            return false;
    }
#pragma endregion

#pragma region MIRROR
    {
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        result = device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)AdressMode::eMirror][(int)SamplerState::eTriLinear].GetAddressOf());
        if (FAILED(result))
            return false;
    }
    {
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        result = device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)AdressMode::eMirror][(int)SamplerState::eBiliniear].GetAddressOf());
        if (FAILED(result))
            return false;
    }
    {
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        result = device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)AdressMode::eMirror][(int)SamplerState::eAnisotropic].GetAddressOf());
        if (FAILED(result))
            return false;
    }
    {
        D3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.BorderColor[0] = 0;
        samplerDesc.BorderColor[1] = 0;
        samplerDesc.BorderColor[2] = 0;
        samplerDesc.BorderColor[3] = 0;
        samplerDesc.MinLOD = 0;
        samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

        result = device->CreateSamplerState(&samplerDesc, mySamplerStates[(int)AdressMode::eMirror][(int)SamplerState::ePoint].GetAddressOf());
        if (FAILED(result))
            return false;
    }
#pragma endregion

    return true;
}
bool ISTE::RenderStateManager::CreateRasterStates()
{
    ID3D11Device* device = myCtx->myDX11->GetDevice();

    HRESULT result;
    {
        D3D11_RASTERIZER_DESC BackfaceCullingRasteDesc;
        BackfaceCullingRasteDesc.FillMode = D3D11_FILL_SOLID;
        BackfaceCullingRasteDesc.CullMode = D3D11_CULL_NONE;
        BackfaceCullingRasteDesc.FrontCounterClockwise = FALSE;
        BackfaceCullingRasteDesc.DepthBias = 20;
        BackfaceCullingRasteDesc.DepthBiasClamp = 2;
        BackfaceCullingRasteDesc.SlopeScaledDepthBias = 5; 
        BackfaceCullingRasteDesc.DepthClipEnable = TRUE;
        BackfaceCullingRasteDesc.ScissorEnable = false;
        BackfaceCullingRasteDesc.MultisampleEnable = false;
        BackfaceCullingRasteDesc.AntialiasedLineEnable = false;


        result = device->CreateRasterizerState(&BackfaceCullingRasteDesc, &myRasterizerStates[(int)RasterizerState::eBiasedRaster]);
        if (FAILED(result))
            return false;
    }
    {
        D3D11_RASTERIZER_DESC BackfaceCullingRasteDesc;
        BackfaceCullingRasteDesc.FillMode = D3D11_FILL_SOLID;
        BackfaceCullingRasteDesc.CullMode = D3D11_CULL_BACK;
        BackfaceCullingRasteDesc.FrontCounterClockwise = FALSE;
        BackfaceCullingRasteDesc.DepthBias = 0;
        BackfaceCullingRasteDesc.DepthBiasClamp = 0;
        BackfaceCullingRasteDesc.SlopeScaledDepthBias = 0;
        BackfaceCullingRasteDesc.DepthClipEnable = TRUE;
        BackfaceCullingRasteDesc.ScissorEnable = false;
        BackfaceCullingRasteDesc.MultisampleEnable = false;
        BackfaceCullingRasteDesc.AntialiasedLineEnable = false;

        result = device->CreateRasterizerState(&BackfaceCullingRasteDesc, &myRasterizerStates[(int)RasterizerState::eBackFaceCulling]);
        if (FAILED(result))
            return false;
    }
    {
        D3D11_RASTERIZER_DESC BackfaceCullingRasteDesc;
        BackfaceCullingRasteDesc.FillMode = D3D11_FILL_SOLID;
        BackfaceCullingRasteDesc.CullMode = D3D11_CULL_FRONT;
        BackfaceCullingRasteDesc.FrontCounterClockwise = FALSE;
        BackfaceCullingRasteDesc.DepthBias = 0;
        BackfaceCullingRasteDesc.DepthBiasClamp = 0;
        BackfaceCullingRasteDesc.SlopeScaledDepthBias = 0;
        BackfaceCullingRasteDesc.DepthClipEnable = TRUE;
        BackfaceCullingRasteDesc.ScissorEnable = false;
        BackfaceCullingRasteDesc.MultisampleEnable = false;
        BackfaceCullingRasteDesc.AntialiasedLineEnable = false;

        result = device->CreateRasterizerState(&BackfaceCullingRasteDesc, &myRasterizerStates[(int)RasterizerState::eFrontFaceCulling]);
        if (FAILED(result))
            return false;
    }
    {
        D3D11_RASTERIZER_DESC BackfaceCullingRasteDesc;
        BackfaceCullingRasteDesc.FillMode = D3D11_FILL_SOLID;
        BackfaceCullingRasteDesc.CullMode = D3D11_CULL_NONE;
        BackfaceCullingRasteDesc.FrontCounterClockwise = FALSE;
        BackfaceCullingRasteDesc.DepthBias = 0;
        BackfaceCullingRasteDesc.DepthBiasClamp = 0;
        BackfaceCullingRasteDesc.SlopeScaledDepthBias = 0;
        BackfaceCullingRasteDesc.DepthClipEnable = TRUE;
        BackfaceCullingRasteDesc.ScissorEnable = false;
        BackfaceCullingRasteDesc.MultisampleEnable = false;
        BackfaceCullingRasteDesc.AntialiasedLineEnable = false;

        result = device->CreateRasterizerState(&BackfaceCullingRasteDesc, &myRasterizerStates[(int)RasterizerState::eNoFaceCulling]);
        if (FAILED(result))
            return false;
    }
    {
        D3D11_RASTERIZER_DESC WireFrameDesc;
        WireFrameDesc.FillMode = D3D11_FILL_WIREFRAME;
        WireFrameDesc.CullMode = D3D11_CULL_BACK;
        WireFrameDesc.FrontCounterClockwise = FALSE;
        WireFrameDesc.DepthBias = 0;
        WireFrameDesc.DepthBiasClamp = 0;
        WireFrameDesc.SlopeScaledDepthBias = 0;
        WireFrameDesc.DepthClipEnable = TRUE;
        WireFrameDesc.ScissorEnable = false;
        WireFrameDesc.MultisampleEnable = false;
        WireFrameDesc.AntialiasedLineEnable = false;

        result = device->CreateRasterizerState(&WireFrameDesc, &myRasterizerStates[(int)RasterizerState::eWireFrameMode]);
        if (FAILED(result))
            return false;
    }


    return true;
}

bool ISTE::RenderStateManager::CreateBlendStates()
{
    ID3D11Device* device = myCtx->myDX11->GetDevice();
    HRESULT result;
    D3D11_BLEND_DESC blendDescs = { 0 };
    
    //Disabled
    {
    blendDescs.RenderTarget[0].BlendEnable = false;
    blendDescs.RenderTarget[0].SrcBlend = D3D11_BLEND_ZERO;
    blendDescs.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
    blendDescs.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDescs.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    blendDescs.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendDescs.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendDescs.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    result = device->CreateBlendState(&blendDescs, myBlendStates[(int)BlendState::eDisabled].GetAddressOf());
    if (FAILED(result))
        return false;
    }

    //Alpha
    {
    blendDescs.RenderTarget[0].BlendEnable = true;
    blendDescs.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDescs.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDescs.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDescs.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDescs.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blendDescs.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
    blendDescs.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    result = device->CreateBlendState(&blendDescs, myBlendStates[(int)BlendState::eAlphaBlend].GetAddressOf());
    if (FAILED(result))
        return false;
    }

    //Addative
    {
    blendDescs.RenderTarget[0].BlendEnable = true;
    blendDescs.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDescs.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
    blendDescs.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendDescs.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDescs.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blendDescs.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
    blendDescs.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    result = device->CreateBlendState(&blendDescs, myBlendStates[(int)BlendState::eAddativeBlend].GetAddressOf());
    if (FAILED(result))
        return false;
    }

    //Add and fade
    {
    blendDescs.RenderTarget[0].BlendEnable = true;
    blendDescs.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendDescs.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
    blendDescs.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; 
    blendDescs.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
    blendDescs.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
    blendDescs.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_MAX;
    blendDescs.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    result = device->CreateBlendState(&blendDescs, myBlendStates[(int)BlendState::eAddAndFadeBackgroundBlend].GetAddressOf());
    if (FAILED(result))
        return false;
    }
    return true;

}

bool ISTE::RenderStateManager::CreateDepthStates()
{
    ID3D11Device* device = myCtx->myDX11->GetDevice();
    HRESULT result;
    D3D11_DEPTH_STENCIL_DESC depthDesc = { 0 };
#pragma region"Write"
    //Less
    {
     
        depthDesc.DepthEnable = true;
        depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthDesc.DepthFunc = D3D11_COMPARISON_LESS;

        result = device->CreateDepthStencilState(&depthDesc, &myDepthStencilStates[(int)DepthState::eDepthLess][(int)ReadWriteState::eWrite]);
        if (FAILED(result))
            return false;
    }
    //LessEqual
    {
        depthDesc = { 0 };
        depthDesc.DepthEnable = true;
        depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

        result = device->CreateDepthStencilState(&depthDesc, &myDepthStencilStates[(int)DepthState::eDepthLessOrEqual][(int)ReadWriteState::eWrite]);
        if (FAILED(result))
            return false;
    }
    //Greater
    {
        depthDesc = { 0 };
        depthDesc.DepthEnable = true;
        depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;  //tge has MASK_ZERO in case it breaks 
        depthDesc.DepthFunc = D3D11_COMPARISON_GREATER;

        result = device->CreateDepthStencilState(&depthDesc, &myDepthStencilStates[(int)DepthState::eDepthGreater][(int)ReadWriteState::eWrite]);
        if (FAILED(result))
            return false;
    }
    //GreaterEqual
    {
        depthDesc = { 0 };
        depthDesc.DepthEnable = true;
        depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

        result = device->CreateDepthStencilState(&depthDesc, &myDepthStencilStates[(int)DepthState::eDepthGreaterOrEqual][(int)ReadWriteState::eWrite]);
        if (FAILED(result))
            return false;
    }

    //AlwaysPass
    {
        depthDesc = { 0 };
        depthDesc.DepthEnable = true;
        depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        depthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

        result = device->CreateDepthStencilState(&depthDesc, &myDepthStencilStates[(int)DepthState::eAlwaysPass][(int)ReadWriteState::eWrite]);
        if (FAILED(result))
            return false;
    }

#pragma endregion

#pragma region"Read"
    //Less
    {

        depthDesc.DepthEnable = true;
        depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthDesc.DepthFunc = D3D11_COMPARISON_LESS;

        result = device->CreateDepthStencilState(&depthDesc, &myDepthStencilStates[(int)DepthState::eDepthLess][(int)ReadWriteState::eRead]);
        if (FAILED(result))
            return false;
    }
    //LessEqual
    {
        depthDesc = { 0 };
        depthDesc.DepthEnable = true;
        depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

        result = device->CreateDepthStencilState(&depthDesc, &myDepthStencilStates[(int)DepthState::eDepthLessOrEqual][(int)ReadWriteState::eRead]);
        if (FAILED(result))
            return false;
    }
    //Greater
    {
        depthDesc = { 0 };
        depthDesc.DepthEnable = true;
        depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;  
        depthDesc.DepthFunc = D3D11_COMPARISON_GREATER;

        result = device->CreateDepthStencilState(&depthDesc, &myDepthStencilStates[(int)DepthState::eDepthGreater][(int)ReadWriteState::eRead]);
        if (FAILED(result))
            return false;
    }
    //GreaterEqual
    {
        depthDesc = { 0 };
        depthDesc.DepthEnable = true;
        depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthDesc.DepthFunc = D3D11_COMPARISON_GREATER_EQUAL;

        result = device->CreateDepthStencilState(&depthDesc, &myDepthStencilStates[(int)DepthState::eDepthGreaterOrEqual][(int)ReadWriteState::eRead]);
        if (FAILED(result))
            return false;
    }

    //AlwaysPass
    {
        depthDesc = { 0 };
        depthDesc.DepthEnable = true;
        depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        depthDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

        result = device->CreateDepthStencilState(&depthDesc, &myDepthStencilStates[(int)DepthState::eAlwaysPass][(int)ReadWriteState::eRead]);
        if (FAILED(result))
            return false;
    }

#pragma endregion


    return true;
}
