#include "FullscreenEffect.h"

#include "ISTE/Graphics/DX11.h"
#include "ISTE/Graphics/Resources/ShaderManager.h"
#include "ISTE/Context.h"

#include <d3d11.h>

ISTE::FullscreenEffect::FullscreenEffect()
{
	myCtx = nullptr;
}

ISTE::FullscreenEffect::~FullscreenEffect() {}

bool ISTE::FullscreenEffect::Init(std::string aPSPath)
{
	myCtx = ISTE::Context::Get();

	std::string vsData;
	if (!myCtx->myDX11->CreateVertexShader("Shaders/Fullscreen_VS.cso", &myVertexShader, vsData))
		return false;

	if (!myCtx->myDX11->CreatePixelShader(aPSPath, &myPixelShader))
		return false;

	return true;
}

void ISTE::FullscreenEffect::Render()
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();
	
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	context->IASetInputLayout(nullptr);
	context->IASetVertexBuffers(0, 0, nullptr, nullptr, nullptr);
	context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);

	context->VSSetShader(myVertexShader.Get(), nullptr, 0);
	context->GSSetShader(nullptr, nullptr, 0);
	context->PSSetShader(myPixelShader.Get(), nullptr, 0);
	//DX11::LogDrawCall();
	context->Draw(3, 0);
}
