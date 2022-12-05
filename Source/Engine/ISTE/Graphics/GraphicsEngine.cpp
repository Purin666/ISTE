#include "GraphicsEngine.h"
#include <d3d11.h> 

#include "ISTE/Context.h"
#include "ISTE/Graphics/DX11.h"

#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Graphics/Resources/ShaderManager.h"
#include "ISTE/Graphics/RenderStateManager.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/ECSB/System.h"

#include "ISTE/Scene/Scene.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/WindowsWindow.h"
#include "ISTE/CU/InputHandler.h"

#include "ISTE/Logger/Logger.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Math/Vec.h" 
#include "ISTE/Math/Matrix4x4.h"
#pragma region systems
#include <ISTE/ComponentsAndSystems/EnemyBehaviourSystem.h>
#include <ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h>
#include <ISTE/Graphics/ComponentAndSystem/VFXSpriteDrawerSystem.h>
#include <ISTE/Graphics/ComponentAndSystem/ModelDrawerSystem.h>
#include <ISTE/Graphics/ComponentAndSystem/CustomDrawerSystem.h>
#include <ISTE/Graphics/ComponentAndSystem/TransperancyDrawerSystem.h>
#include <ISTE/Graphics/ComponentAndSystem/AnimationDrawerSystem.h>
#include <ISTE/Graphics/ComponentAndSystem/TransformSystem.h>
#include <ISTE/Graphics/ComponentAndSystem/CameraSystem.h>
#include <ISTE/Graphics/ComponentAndSystem/LightDrawerSystem.h>
#include <ISTE/Graphics/ComponentAndSystem/DecalDrawerSystem.h>
#include <ISTE/Text/TextSystem.h>
#pragma endregion
 


ISTE::GraphicsEngine::GraphicsEngine()
{
}

ISTE::GraphicsEngine::~GraphicsEngine()
{
    //mySSAOContext->Release();
}

bool ISTE::GraphicsEngine::Init()
{
    myCtx = Context::Get();

    if (!CreateBuffers())
    {
#ifdef _DEBUG
        std::cout << "Graphics Engine    -   Failed Creating Render Buffers in Graphics Engine" << std::endl;
        Logger::DebugOutputError(L"Graphics Engine    -   Failed Creating Render Buffers in Graphics Engine");
#endif
        return false;
    }

    myModelDrawer               = myCtx->mySystemManager->GetSystem<ModelDrawerSystem>();
    mySpriteDrawer              = myCtx->mySystemManager->GetSystem<SpriteDrawerSystem>();
    myVFXSpriteDrawer           = myCtx->mySystemManager->GetSystem<VFXSpriteDrawerSystem>();
    myAnimationDrawerSystem     = myCtx->mySystemManager->GetSystem<AnimationDrawerSystem>();
    myCustomDrawerSystem        = myCtx->mySystemManager->GetSystem<CustomDrawerSystem>();
    //myTransperancyDrawerSystem  = myCtx->mySystemManager->GetSystem<TransperancyDrawerSystem>();
    myTransformSystem           = myCtx->mySystemManager->GetSystem<TransformSystem>();
    myCameraSystem              = myCtx->mySystemManager->GetSystem<CameraSystem>();
    myTextSystem                = myCtx->mySystemManager->GetSystem<TextSystem>();
    myLightDrawerSystem         = myCtx->mySystemManager->GetSystem<LightDrawerSystem>();
    myDecalDrawerSystem         = myCtx->mySystemManager->GetSystem<DecalDrawerSystem>();


    myCtx->mySystemManager->GetSystem<EnemyBehaviourSystem>()->Init(); //shouldnt be here at all

    myModelDrawer->Init("Shaders/InstancedModel_VS.cso", "Shaders/InstancedGBuffer_PS.cso");
    myAnimationDrawerSystem->Init("Shaders/AnimatedDefault_VS.cso", "Shaders/GBuffer_PS.cso");
    myLightDrawerSystem->Init();
    myCustomDrawerSystem->Init();
    //myTransperancyDrawerSystem->Init();
    myTransformSystem->Init();
    mySpriteDrawer->Init();
    myVFXSpriteDrawer->Init();
    myCameraSystem->Init();
    myTextSystem->Init();
    myDebugDrawer.Init();
    myDecalDrawerSystem->Init();
    mySSAOEffect.Init();
    myMotionBlurEffect.Init();
    myChromaticAberrationEffect.Init();
    myRadialBlurEffect.Init();
    
    //myEditorCam.Init();
    //myEditorCam.SetPerspectiveProjection(90, myCtx->myWindow->GetResolution(), 0.5, 500);
    my2DWorldToClip = CU::Matrix4x4f::CreateOrthographicMatrix(-1.f, 1.f, -1.f, 1.f, 0, 10);
    return true;
}

void ISTE::GraphicsEngine::Render()
{
    //myEditorCam.Update(myCtx->myTimer->GetDeltaTime());
    myCameraSystem->Update(); 

    //Rendering  
    myFrameBufferData.myPrevWorldToClip = myPrevWTCMatrix;
    BindLightBuffer(); 
    BindFrameBuffer();
    BindPostProcessBuffer();
    myPrevWTCMatrix = myFrameBufferData.myWorldToClip;

    //temp gone untill alpha thing
    ////depth prepass
    myCtx->myDX11->myGBuffer.ClearTextures(); 
    myCtx->myDX11->myGBuffer.SetAsActiveTarget(myCtx->myDX11->GetDepthBuffer()->GetDepthStencilRTV());
    myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthLess, ReadWriteState::eWrite);
    //RenderDepth();

    //Models Animation Sprite and text wil be rendered to GBuffer
    DrawToGBuffer();
    
    //draws decals to the gbuffers textures
    myDecalDrawerSystem->Draw();

    mySSAOEffect.Draw();

    //Binds Scene Texture and sets light to the scen
    myCtx->myDX11->GetSceneTarget().SetActiveTarget(myCtx->myDX11->GetDepthBuffer()->GetDepthStencilRTV());
    myCtx->myRenderStateManager->SetSampleState(AdressMode::eClamp, SamplerState::ePoint, 0);
    myCtx->myTextureManager->PsBindTexture(myCtx->myTextureManager->LoadTexture(L"../Assets/Sprites/cube_1024_preblurred_angle3_Skansen3.dds", false, false), 3);
    myCtx->myDX11->myGBuffer.PSSetAllResources(4);
    DrawLight();

     
    //Forward draw custom, Transparency, Sprite, and Text
    myCtx->myDX11->SetSceneAndEntityTextureActive(myCtx->myDX11->GetDepthBuffer()->GetDepthStencilRTV()); 
    myCtx->myRenderStateManager->SetRasterState(RasterizerState::eBackFaceCulling);
    myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthLessOrEqual, ReadWriteState::eWrite);
    myCtx->myRenderStateManager->SetBlendState(BlendState::eDisabled);
    myDecalDrawerSystem->DrawDebugLines();
    mySpriteDrawer->RenderBillboards();
    mySpriteDrawer->Render3DSprites();
    
    myCustomDrawerSystem->Draw();
    //myCtx->myDX11->mySceneTexture.SetActiveTarget(nullptr);
    myVFXSpriteDrawer->Render3DSprites();
    //myCtx->myDX11->mySceneTexture.SetActiveTarget(myCtx->myDX11->GetDepthBuffer()->GetDepthStencil());
    //myTransperancyDrawerSystem->Draw();
    // clear depth after customdrawer and transparency
    //myCtx->myDX11->GetDepthBuffer()->Clear(1);

     
    myVFXSpriteDrawer->Render2DSprites();
    //PostProcessing
    PostProcess();
    myDebugDrawer.Draw();

    //text and 2d sprite rendering
    mySpriteDrawer->Render2DSprites();
    myTextSystem->Draw();
}


void ISTE::GraphicsEngine::RenderDepth()
{
    myModelDrawer->DrawDepth();
    myAnimationDrawerSystem->DrawDepth();

    //they shouldnt do this honestly
    //mySpriteDrawer->DrawDepth();
    //mySpriteDrawer->RenderBillboardDepth();
    //mySpriteDrawer->Render3DSpriteDepth();
}

void ISTE::GraphicsEngine::RenderDepth(std::set<EntityID>& aIDList)
{
    myModelDrawer->DrawDepth(aIDList);
    myAnimationDrawerSystem->DrawDepth(aIDList);
}

void ISTE::GraphicsEngine::CopyToBackBuffer()
{
    DX11* dx11 = myCtx->myDX11;

    //copy scene to back buffer
    dx11->myBackBuffer.SetActiveTarget();

    // selecting renderpass
    if (mySelectedRenderPass != GBufferTexture::eCount)
        dx11->myGBuffer.SetAsResourceOnSlot(mySelectedRenderPass, 0);
    else
        dx11->mySceneTexture.PsBind(0);

    // copy render
    myCtx->myShaderManager->GetFullscreenEffect(ISTE::eFullscreenEffects::ePPCopy).Render();
}

ISTE::GPUOutData ISTE::GraphicsEngine::Pick(const CU::Vec2Ui& aMouseCoord)
{
    auto resolution = myCtx->myDX11->GetBackBufferRes();
    if ((aMouseCoord.x < 0 || aMouseCoord.x > resolution.x) ||
        (aMouseCoord.y < 0 || aMouseCoord.y > resolution.y))
        return GPUOutData();

    ID3D11DeviceContext* dCtx = myCtx->myDX11->GetContext();
    ID3D11Device* dDvc = myCtx->myDX11->GetDevice();

    ID3D11Texture2D* ePixel = nullptr;
    ID3D11Texture2D* cDPixel = nullptr;

    D3D11_TEXTURE2D_DESC textureDesc = {};
    textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
    textureDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
    textureDesc.Usage = D3D11_USAGE_STAGING;
    textureDesc.Width = 1;
    textureDesc.Height = 1;
    textureDesc.ArraySize = 1;
    textureDesc.SampleDesc.Count = 1;

    HRESULT res = dDvc->CreateTexture2D(&textureDesc, nullptr, &cDPixel);
    if (FAILED(res))
    {
        cDPixel->Release();
        //failed to create pxiel outtexture
        return GPUOutData();
    }
    res = dDvc->CreateTexture2D(&textureDesc, nullptr, &ePixel);
    if (FAILED(res))
    {
        ePixel->Release();
        cDPixel->Release();
        //failed to create pxiel outtexture
        return GPUOutData();
    }

    D3D11_BOX srcBox{};
    srcBox.left = aMouseCoord.x;
    srcBox.right = aMouseCoord.x + 1;
    srcBox.bottom = aMouseCoord.y + 1;
    srcBox.top = aMouseCoord.y;
    srcBox.front = 0;
    srcBox.back = 1;


    ID3D11Resource* srcEnt; 
    ID3D11Resource* srcCData;
    myCtx->myDX11->myGBuffer.GetSRV(GBufferTexture::eEntityData)->GetResource(&srcEnt);
    myCtx->myDX11->myGBuffer.GetSRV(GBufferTexture::eCustomData)->GetResource(&srcCData);

    //sketch
    dCtx->CopySubresourceRegion(
        cDPixel,
        0, 0, 0, 0,
        srcCData, 0,
        &srcBox);

    dCtx->CopySubresourceRegion(
        ePixel,
        0, 0, 0, 0,
        srcEnt, 0,
        &srcBox);

    D3D11_MAPPED_SUBRESOURCE eMsr = {}; 
    D3D11_MAPPED_SUBRESOURCE cdMsr = {};
    dCtx->Map(ePixel, 0, D3D11_MAP_READ, 0, &eMsr);
    dCtx->Map(cDPixel, 0, D3D11_MAP_READ, 0, &cdMsr);


    GPUOutData out;
    //std::cout << out.myId - 1 << std::endl;
    out.myId    = ((float*)eMsr.pData)[0];
    out.myDepth = ((float*)cdMsr.pData)[1];
    out.myId -= 1;

    ePixel->Release();
    cDPixel->Release();
    srcEnt->Release();
    srcCData->Release();

    return out;
}

void ISTE::GraphicsEngine::BindInitBuffer()
{
}

void ISTE::GraphicsEngine::PrepareRenderCommands()
{
    myTransformSystem->UpdateTransform(); 
    myModelDrawer->PrepareBatchedRenderCommands();
    myCtx->mySystemManager->GetSystem<EnemyBehaviourSystem>()->UpdateHealthBarDrawCalls();
    mySpriteDrawer->PrepareRenderCommands();
    myVFXSpriteDrawer->PrepareRenderCommands();
    //myTransperancyDrawerSystem->PrepareRenderCommands();
    myCustomDrawerSystem->PrepareRenderCommands();
    myAnimationDrawerSystem->PrepareRenderCommands();
    myTextSystem->PrepareRenderCommands();
    myLightDrawerSystem->PrepareLightCommands();
    myDecalDrawerSystem->PrepareRenderCommands();
}

void ISTE::GraphicsEngine::DrawToGBuffer()
{
    //myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthLessOrEqual, ReadWriteState::eRead);
    //mySpriteDrawer->Render3DSprites();
    myModelDrawer->Draw();
    myAnimationDrawerSystem->Draw();
}

void ISTE::GraphicsEngine::DrawLight()
{
    mySSAOEffect.GetRenderTarget().PsBind(11);
    myLightDrawerSystem->RenderLights();
}

 

void ISTE::GraphicsEngine::PostProcess()
{
    //Post processing
    myCtx->myDX11->myIntermediateTexture.SetActiveTarget();
    myCtx->myDX11->mySceneTexture.PsBind(0);
    myCtx->myRenderStateManager->SetDepthState(DepthState::eDepthLessOrEqual);
    myCtx->myRenderStateManager->SetBlendState(BlendState::eDisabled);
    myCtx->myDX11->myGBuffer.PSSetAllResources(4,false);
    
    ID3D11ShaderResourceView* nullsrv = nullptr;
    myRadialBlurEffect.Draw(); 
    
    myCtx->myDX11->GetContext()->PSSetShaderResources(0, 1, &nullsrv);
    myCtx->myDX11->mySceneTexture.SetActiveTarget();
    myCtx->myDX11->myIntermediateTexture.PsBind(0); 
    

    BindFogBuffer();
    myCtx->myShaderManager->GetFullscreenEffect(eFullscreenEffects::ePPFog).Render();

    DownAndUpSample();

    myCtx->myDX11->mySceneTexture.SetActiveTarget();
    myCtx->myDX11->myIntermediateTexture.PsBind(0);

    myChromaticAberrationEffect.Draw();
    myCtx->myDX11->GetContext()->PSSetShaderResources(0, 1, &nullsrv);
    myCtx->myDX11->myIntermediateTexture.SetActiveTarget();
    myCtx->myDX11->mySceneTexture.PsBind(0); 

    myCtx->myShaderManager->GetFullscreenEffect(ISTE::eFullscreenEffects::ePPTonemap).Render();


    myCtx->myDX11->GetContext()->PSSetShaderResources(0, 1, &nullsrv);
    myCtx->myDX11->GetContext()->PSSetShaderResources(10, 1, &nullsrv);
    myCtx->myDX11->SetSceneAndEntityTextureActive();
    myCtx->myDX11->myIntermediateTexture.PsBind(0);
    myCtx->myShaderManager->GetFullscreenEffect(ISTE::eFullscreenEffects::ePPCopy).Render();

}

void ISTE::GraphicsEngine::ClearCommands()
{
    myModelDrawer->ClearCommands();
    myAnimationDrawerSystem->ClearCommands();
    mySpriteDrawer->ClearCommands();
    myVFXSpriteDrawer->ClearCommands();
    myTextSystem->ClearCommands();
    //myTransperancyDrawerSystem->ClearCommands();
    myDebugDrawer.ClearDynamicCommands();
    myLightDrawerSystem->ClearRenderCommands();
    myDecalDrawerSystem->ClearRenderCommands();
}

bool ISTE::GraphicsEngine::CreateBuffers()
{
    ID3D11Device* device = myCtx->myDX11->GetDevice();

    HRESULT result;
    //Frame Buffer init
    {
        D3D11_BUFFER_DESC frameBufferDesc = {};
        frameBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
        frameBufferDesc.ByteWidth = sizeof(FrameBuffer);
        frameBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        frameBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        result = device->CreateBuffer(&frameBufferDesc, NULL, myFrameBuffer.GetAddressOf());
        if (FAILED(result))
        {
#ifdef _DEBUG
            std::cout << "Graphics ENGINE    -   Failed Creating Frame Buffer" << std::endl;
            Logger::DebugOutputError(L"Graphics ENGINE    -  Failed Creating Frame Buffer");
#endif 
            return false;
        }
    }
    //Light Buffer init
    {
        D3D11_BUFFER_DESC lightBuffer = {};
        lightBuffer.Usage = D3D11_USAGE_DYNAMIC;
        lightBuffer.ByteWidth = sizeof(LightBuffer);
        lightBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        lightBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        result = device->CreateBuffer(&lightBuffer, NULL, myLightBuffer.GetAddressOf());
        if (FAILED(result))
        {
#ifdef _DEBUG
            std::cout << "Graphics ENGINE    -   Failed Creating Light Buffer" << std::endl;
            Logger::DebugOutputError(L"Graphics ENGINE    -   Failed Creating Light Buffer");
#endif 
            return false;
        }
    }
    //PP Buffer init
    {
        D3D11_BUFFER_DESC ppDesc = {};
        ppDesc.Usage = D3D11_USAGE_DYNAMIC;
        ppDesc.ByteWidth = sizeof(PostProcessBuffer);
        ppDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        ppDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        result = device->CreateBuffer(&ppDesc, NULL, myPostProcessBuffer.GetAddressOf());
        if (FAILED(result))
        {

#ifdef _DEBUG
            std::cout << "Graphics ENGINE    -   ailed Creating PostProcess Buffer" << std::endl;
            Logger::DebugOutputError(L"Graphics ENGINE    -   ailed Creating PostProcess Buffer");
#endif  
            return false;
        }
    }
    //fog Buffer init
    {
        D3D11_BUFFER_DESC ppDesc = {};
        ppDesc.Usage = D3D11_USAGE_DYNAMIC;
        ppDesc.ByteWidth = sizeof(FogBuffer);
        ppDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        ppDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
        result = device->CreateBuffer(&ppDesc, NULL, myFogBuffer.GetAddressOf());
        if (FAILED(result))
        {
#ifdef _DEBUG
            std::cout << "Graphics ENGINE    -   Failed Creating Fog Buffer" << std::endl;
            Logger::DebugOutputError(L"Graphics ENGINE    -   Failed Creating Fog Buffer");
#endif  
            return false;
        }
    }

    //InitBuffer        -       not really used yet
    {
    //D3D11_BUFFER_DESC lightBuffer = {};
    //lightBuffer.Usage = D3D11_USAGE_DYNAMIC;
    //lightBuffer.ByteWidth = sizeof(LightBuffer);
    //lightBuffer.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    //lightBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    //result = device->CreateBuffer(&lightBuffer, NULL, myLightBuffer.GetAddressOf());
    //if (FAILED(result))
    //    return false;
    }


    return true;
}

 

void ISTE::GraphicsEngine::BindFrameBuffer()
{
    ID3D11DeviceContext* context = myCtx->myDX11->GetContext();
     
    myFrameBufferData.myResolution = { (float)myCtx->myDX11->GetBackBufferRes().x, (float)myCtx->myDX11->GetBackBufferRes().y };
    myFrameBufferData.myTimings.x = myCtx->myTimeHandler->GetDeltaTime();
    myFrameBufferData.myTimings.y = myCtx->myTimeHandler->GetTotalTime();

    //moving toClip space matrix somehwere would maybe be a good idea 
    myFrameBufferData.myWorldToClip = myCamera->GetWorldToClip();
    myFrameBufferData.myWorldToView = myCamera->GetTransformNonConst();
    myFrameBufferData.myCamPos = myCamera->GetPosition();

    
    D3D11_MAPPED_SUBRESOURCE resource = { 0 };
    HRESULT res = context->Map(myFrameBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    if (FAILED(res))
        return;
    myFrameBufferData.my2DWorldToView = my2DWorldToClip;
    memcpy(resource.pData, &myFrameBufferData, sizeof(FrameBuffer));
    context->Unmap(myFrameBuffer.Get(), 0);

    context->VSSetConstantBuffers(1, 1, myFrameBuffer.GetAddressOf());
    context->PSSetConstantBuffers(1, 1, myFrameBuffer.GetAddressOf());
}

void ISTE::GraphicsEngine::BindLightBuffer()
{
    ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

    //myCtx->myTextureManager->PsBindTexture(myAmbientLight.myCubeMapId, 3);
    

    D3D11_MAPPED_SUBRESOURCE resource = { 0 };
    HRESULT res = context->Map(myLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    if (FAILED(res))
        return;
    memcpy(resource.pData, &myLightBufferData, sizeof(LightBuffer));
    context->Unmap(myLightBuffer.Get(), 0);

    context->PSSetConstantBuffers(0, 1, myLightBuffer.GetAddressOf());
}

void ISTE::GraphicsEngine::BindLightBuffer(const CU::Matrix4x4f& aLightSpaceMatrix, const CU::Vec3f& aDlDir)
{
    ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

    //myCtx->myTextureManager->PsBindTexture(myAmbientLight.myCubeMapId, 3);

    LightBuffer LBdata;
    LBdata.myALGroundColorAndIntensity = { 0.75f,0.85f,0.75f,0.025f };
    LBdata.myALSkyColorAndIntensity = { 1.f,1.f,0.68f ,0.025f};
    LBdata.myDLColorAndIntensity = { 1,1,1,1 };
    LBdata.myDLdir = aDlDir;
    LBdata.myDLdir.Normalize();
    LBdata.myLightSpaceMatrix = aLightSpaceMatrix;
    D3D11_MAPPED_SUBRESOURCE resource = { 0 };
    HRESULT res = context->Map(myLightBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    if (FAILED(res))
        return;
    memcpy(resource.pData, &LBdata, sizeof(LightBuffer));
    context->Unmap(myLightBuffer.Get(), 0);

    context->PSSetConstantBuffers(0, 1, myLightBuffer.GetAddressOf());
}

void ISTE::GraphicsEngine::BindPostProcessBuffer()
{
    ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

    auto& database = myCtx->mySceneHandler->GetActiveScene().GetDatabase();
     
    database.TryGet("BloomBlending", myPPdata.myBloomBlending);
    database.TryGet("Saturation", myPPdata.mySaturation);
    database.TryGet("Exposure", myPPdata.myExposure);
    database.TryGet("Contrast", myPPdata.myContrast);
    database.TryGet("Tint", myPPdata.myTint);
    database.TryGet("Blackpoint", myPPdata.myBlackpoint);

    D3D11_MAPPED_SUBRESOURCE resource = { 0 };
    HRESULT res = context->Map(myPostProcessBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    if (FAILED(res))
        return;
    memcpy(resource.pData, &myPPdata, sizeof(PostProcessBuffer));
    context->Unmap(myPostProcessBuffer.Get(), 0);

    context->VSSetConstantBuffers(8, 1, myPostProcessBuffer.GetAddressOf());
    context->PSSetConstantBuffers(8, 1, myPostProcessBuffer.GetAddressOf());

}

void ISTE::GraphicsEngine::BindFogBuffer() 
{
    ID3D11DeviceContext* context = myCtx->myDX11->GetContext(); 

    auto& database = myCtx->mySceneHandler->GetActiveScene().GetDatabase();
    database.TryGet("FogColor", myFogBufferData.myFogColor);
    database.TryGet("FogStartDist", myFogBufferData.myFogStartDist);
    database.TryGet("FogHighlightColor", myFogBufferData.myFogHighlightColor);
    database.TryGet("FogGlobalDensity", myFogBufferData.myFogGlobalDensity);
    database.TryGet("FogHeightFalloff", myFogBufferData.myFogHeightFalloff);


    D3D11_MAPPED_SUBRESOURCE resource = { 0 };
    HRESULT res = context->Map(myFogBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    if (FAILED(res))
        return;
    memcpy(resource.pData, &myFogBufferData, sizeof(FogBuffer));
    context->Unmap(myFogBuffer.Get(), 0);
    context->PSSetConstantBuffers(2, 1, myFogBuffer.GetAddressOf());
}

void ISTE::GraphicsEngine::DownAndUpSample()
{
    DX11* dx11 = myCtx->myDX11;
    const size_t count = 4; // "myDownAndUpSampleTexture.size()-1"

    myCtx->myRenderStateManager->SetBlendState(ISTE::BlendState::eDisabled);
    myCtx->myRenderStateManager->SetSampleState(ISTE::AdressMode::eClamp, ISTE::SamplerState::eAnisotropic);

    // use intermediate texture and DownSample to half resolution
    myCtx->myDX11->myDownAndUpSampleTextures[0].SetActiveTarget();
    myCtx->myDX11->mySceneTexture.PsBind(0);
    myCtx->myShaderManager->GetFullscreenEffect(ISTE::eFullscreenEffects::ePPDownSample).Render();

    // use first half resolution to go down to lowest res
    for (size_t i = 0; i < count; ++i)
    {
        dx11->myDownAndUpSampleTextures[i + 1].SetActiveTarget(); // next render target
        dx11->myDownAndUpSampleTextures[i].PsBind(0); // previous written to target
        myCtx->myShaderManager->GetFullscreenEffect(ISTE::eFullscreenEffects::ePPDownSample).Render();
    }

    ID3D11ShaderResourceView* nullsrv = nullptr;
    dx11->GetContext()->PSSetShaderResources(0, 1, &nullsrv);

    // reverse with alpha blend
    myCtx->myRenderStateManager->SetBlendState(ISTE::BlendState::eAlphaBlend);
    for (size_t i = count; i > 0; --i)
    {
        dx11->myDownAndUpSampleTextures[i - 1].SetActiveTarget(); // next render target
        dx11->myDownAndUpSampleTextures[i].PsBind(0); // previous written to target
        myCtx->myShaderManager->GetFullscreenEffect(ISTE::eFullscreenEffects::ePPUpSample).Render();
    }

    // use half resolution texture and UpSample to intermediate texture
    myCtx->myDX11->myIntermediateTexture.SetActiveTarget();
    myCtx->myDX11->myDownAndUpSampleTextures[0].PsBind(0);
    myCtx->myShaderManager->GetFullscreenEffect(ISTE::eFullscreenEffects::ePPUpSample).Render();

    myCtx->myRenderStateManager->SetBlendState(ISTE::BlendState::eDisabled);
    // done
}

void ISTE::GraphicsEngine::SelectRenderPass()
{
    auto input = Context::Get()->myInputHandler;
    const bool tab = input->IsKeyDown(VK_TAB) && input->IsKeyHeldDown(VK_RBUTTON);
    const bool f6 = input->IsKeyDown(VK_F6);
    if (tab || f6)
    {
        int i = (int)mySelectedRenderPass + 1;
        if (i == (int)GBufferTexture::eCount + 1)
            i = 0;
        mySelectedRenderPass = (GBufferTexture)i;
    }
}