#include "ChromaticAberrationEffect.h"
#include <d3d11.h>

#include "ISTE/Graphics/DX11.h"
#include "ISTE/Graphics/Resources/ShaderManager.h"
#include "ISTE/Context.h"
#include "ISTE/CU/InputHandler.h"
#include "ISTE/WindowsWindow.h"
#include "ISTE/Time/TimeHandler.h"

ISTE::ChromaticAberrationEffect::ChromaticAberrationEffect()
{
}

ISTE::ChromaticAberrationEffect::~ChromaticAberrationEffect()
{
}

bool ISTE::ChromaticAberrationEffect::Init()
{
	myCtx = Context::Get();

	if (!InitBuffers())
		return false;
	return true;
}

bool ISTE::ChromaticAberrationEffect::Draw()
{
    if (!myEnabledFlag)
    { 
        myCtx->myShaderManager->GetFullscreenEffect(eFullscreenEffects::ePPCopy).Render();
		return false;
    } 

    BindBuffers();
    switch (myAberrationType){
        case Type::eMouseFocus:
            myCtx->myShaderManager->GetFullscreenEffect(eFullscreenEffects::ePPChromaticAbeMouseFocus).Render();
            break;
        case Type::eRadial :
            myCtx->myShaderManager->GetFullscreenEffect(eFullscreenEffects::ePPChromaticAbeRadial).Render();
            break;
    }

	return true;
}

void ISTE::ChromaticAberrationEffect::BindBuffers()
{
    D3D11_MAPPED_SUBRESOURCE resource;
    ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

    HRESULT res = context->Map(myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
    if (FAILED(res))
        return;
    
    POINT mP;
    GetCursorPos(&mP); 
    HWND hwnd = Context::Get()->myWindow->GetWindowsHandle(); 
    ScreenToClient(hwnd, &mP); 
    if (mP.x < 0) mP.x = 0;
    if (mP.y < 0) mP.y = 0; 

    ChromaticAberrationData* OBdata = (ChromaticAberrationData*)(resource.pData);
    *OBdata = myBufferData;
    myBufferData.myMouseX = (float)mP.x;
    myBufferData.myMouseY = (float)mP.y;

    context->Unmap(myBuffer.Get(), 0);
    context->PSSetConstantBuffers(2, 1, myBuffer.GetAddressOf());
}

bool ISTE::ChromaticAberrationEffect::InitBuffers()
{
    D3D11_BUFFER_DESC desc = {};
    desc.Usage = D3D11_USAGE_DYNAMIC;
    desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    desc.ByteWidth = sizeof(ChromaticAberrationData);

    HRESULT res = myCtx->myDX11->GetDevice()->CreateBuffer(&desc, 0, myBuffer.GetAddressOf());
    if (FAILED(res))
        return false;

    return true;
}
