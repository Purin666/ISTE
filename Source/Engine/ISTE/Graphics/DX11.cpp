#include "DX11.h"
#include "ISTE/Context.h"
#include <d3d11.h> 
#include <fstream>
#include "ISTE/Logger/Logger.h"
#include "ISTE/Context.h"
#include "ISTE/Graphics/GraphicsEngine.h"

#include "ISTE/CU/MemTrack.hpp"

bool ISTE::DX11::CreateVertexShader(const std::string& aVSPath, ID3D11VertexShader** aVsAdRef, std::string& someVsData)
{
	ID3D11Device* aDevice = Context::Get()->myDX11->GetDevice();

	//collection of data
	std::ifstream vsFile(aVSPath, std::ios::binary);
	someVsData = { std::istreambuf_iterator<char>(vsFile), std::istreambuf_iterator<char>() };
	vsFile.close();

	//creation of shader
	HRESULT result = aDevice->CreateVertexShader(someVsData.data(), someVsData.size(), nullptr, aVsAdRef);
	if (FAILED(result))
		return false;

	return true;
}

bool ISTE::DX11::CreatePixelShader(const std::string& aPSPath, ID3D11PixelShader** aPsAdRef)
{
	ID3D11Device* aDevice = Context::Get()->myDX11->GetDevice();

	//collection of data
	std::ifstream psFile;
	psFile.open(aPSPath, std::ios::binary);
	std::string psData = { std::istreambuf_iterator<char>(psFile), std::istreambuf_iterator<char>() };
	psFile.close();

	//creation of shader
	HRESULT result = aDevice->CreatePixelShader(psData.data(), psData.size(), nullptr, aPsAdRef);
	if (FAILED(result))
		return false;

	return true;
}
 

ISTE::DX11::DX11()
{
}

ISTE::DX11::~DX11()
{
	delete myViewPort;
}

bool ISTE::DX11::Init(const HWND aWindowsHandle)
{ 
	HRESULT result;
	//SWAPCHAIN
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.OutputWindow = aWindowsHandle;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.Windowed = true;

		UINT creationFlags = 0;
#ifdef _DEBUG
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif // _DEBUG

		//#if defined(REPORT_DX_WARNINGS)
		//#endif

		result = D3D11CreateDeviceAndSwapChain(
			nullptr,
			D3D_DRIVER_TYPE_HARDWARE,
			nullptr,
			creationFlags,
			nullptr,
			0,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			&mySwapChain,
			&myDevice,
			nullptr,
			&myContext
		);
	}
	// 

	//BSCKBUFFER, DEPTHBUFFER & VIEWPORT & GBUFFER
	{
		ID3D11Texture2D* backBufferTexture = nullptr;

		result = mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBufferTexture);
		if (FAILED(result))
		{
			backBufferTexture->Release();
			return false;
		}
		myBackBuffer.Create(backBufferTexture);
		backBufferTexture->Release();

		unsigned int sizeX = myBackBuffer.GetWidth();
		unsigned int sizeY = myBackBuffer.GetHeight();

		myDepthBuffer.Create(sizeX, sizeY);

		//VIEWPORT
		myBackBuffer.SetActiveTarget(myDepthBuffer.GetDepthStencilRTV());


		myViewPort = new D3D11_VIEWPORT();
		myViewPort->TopLeftX = 0;
		myViewPort->TopLeftY = 0;
		myViewPort->Width = static_cast<float>(sizeX);
		myViewPort->Height = static_cast<float>(sizeY);
		myViewPort->MinDepth = 0.0f;
		myViewPort->MaxDepth = 1.0f;
		myContext->RSSetViewports(1, myViewPort);
		
		myGBuffer.Create(CU::Vec2Ui(sizeX, sizeY));
		myIntermediateGBuffer.Create(CU::Vec2Ui(sizeX, sizeY));
	}


	// Intermediate, Down and Up sample textures
	{
		const unsigned int sizeX = myBackBuffer.GetWidth();
		const unsigned int sizeY = myBackBuffer.GetHeight();
		auto format = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
		myIntermediateTexture.Create(sizeX, sizeY, format);
		mySceneTexture.Create(sizeX, sizeY, format);
		int div = 2;
		for (size_t i = 0; i < 5; i++)
		{
			myDownAndUpSampleTextures[i].Create(sizeX / div, sizeY / div, format);
			div *= 2;
		} 
	}
	// 
	return true;
}

void ISTE::DX11::BeginFrame()
{
	myContext->ClearState();

	myContext->ClearRenderTargetView(myBackBuffer.GetRTV(), &myVoidColor.x);
	myContext->ClearDepthStencilView(myDepthBuffer.GetDepthStencilRTV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//myContext->ClearRenderTargetView(myIntermediateTexture.GetRTV(), &myVoidColor.x);
	for (auto& renderTarget : myDownAndUpSampleTextures)
	{
		myContext->ClearRenderTargetView(renderTarget.GetRTV(), &myVoidColor.x);
	}

	mySceneTexture.Clear();
	myIntermediateTexture.Clear();
}

void ISTE::DX11::EndFrame()
{
	mySwapChain->Present(0, 0);
}

ID3D11DeviceContext* ISTE::DX11::GetNewThreadContext() //Don't drop this thing.
{
	UINT creationFlags = 0;
	ComPtr<ID3D11DeviceContext> context;
	myDevice->CreateDeferredContext(creationFlags, &context);
	return context.Get();
}

void ISTE::DX11::EndThreadContext(ID3D11DeviceContext* aDeferredContext)
{
	ID3D11CommandList* commandList = NULL;
	aDeferredContext->FinishCommandList(false, &commandList);
	myContext.Get()->ExecuteCommandList(commandList, false);
	return;
} 

void ISTE::DX11::SetSceneAndEntityTextureActive(ID3D11DepthStencilView* dsv)
{
	ID3D11RenderTargetView* rtv[2] = {
		mySceneTexture.GetRTV(),
		myGBuffer.GetRTV(GBufferTexture::eEntityData)
	};

	myContext->OMSetRenderTargets(2, rtv, dsv);
}

void ISTE::DX11::SetViewPortActive()
{
	myContext->RSSetViewports(1, myViewPort);
}

void ISTE::DX11::SetResolution(CU::Vec2Ui aNewRes)
{ 
	//sets everything to null
	ID3D11RenderTargetView* nullRTV[1] = { nullptr };
	myContext->OMSetRenderTargets(1, nullRTV, nullptr);
	myContext->OMSetDepthStencilState(0, 0);
	myContext->ClearState();

	Context::Get()->myGraphicsEngine->mySSAOEffect.mySSAOTarget.~RenderTarget();
	myBackBuffer.~RenderTarget();
	myDepthBuffer.~DepthStencilTarget();
	myGBuffer.~GBuffer();
	myIntermediateGBuffer.~GBuffer();
	mySceneTexture.~RenderTarget();
	myIntermediateTexture.~RenderTarget();
	for (size_t i = 0; i < 5; i++)
	{
		myDownAndUpSampleTextures[i].~RenderTarget();
	}

	myBackBuffer = RenderTarget();
	myDepthBuffer = DepthStencilTarget();
	myGBuffer = GBuffer();
	myIntermediateGBuffer = GBuffer();
	mySceneTexture = RenderTarget();
	myIntermediateTexture = RenderTarget();
	Context::Get()->myGraphicsEngine->mySSAOEffect.mySSAOTarget = RenderTarget();
	for (size_t i = 0; i < 5; i++)
	{
		myDownAndUpSampleTextures[i] = RenderTarget();
	}

	myContext->Flush();
	HRESULT res = mySwapChain->ResizeBuffers(0, aNewRes.x, aNewRes.y, DXGI_FORMAT_UNKNOWN, 0);
	if (FAILED(res))
	{
#ifdef _DEBUG
		std::cout << "DX11    -   Failed to resize swapchain" << std::endl;
		Logger::DebugOutputError(L"DX11    -   Failed to resize swapchain");
#endif 
		return;
	}
	ID3D11Texture2D* swapchainTexture = nullptr;
	mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&swapchainTexture);

	myBackBuffer.Create(swapchainTexture);
	swapchainTexture->Release();

	unsigned int sizeX = myBackBuffer.GetWidth();
	unsigned int sizeY = myBackBuffer.GetHeight();

	myDepthBuffer.Create(sizeX, sizeY);
	myBackBuffer.SetActiveTarget(myDepthBuffer.GetDepthStencilRTV());
	myGBuffer.Create(CU::Vec2Ui(sizeX, sizeY)); 
	myIntermediateGBuffer.Create(CU::Vec2Ui(sizeX, sizeY));
	// Intermediate, Down and Up sample textures
	Context::Get()->myGraphicsEngine->mySSAOEffect.mySSAOTarget.Create(sizeX, sizeY, DXGI_FORMAT_R32G32B32A32_FLOAT);
	auto format = DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT;
	myIntermediateTexture.Create(sizeX, sizeY, format);
	mySceneTexture.Create(sizeX, sizeY, format);
	int div = 2;
	for (size_t i = 0; i < 5; i++)
	{
		myDownAndUpSampleTextures[i].Create(sizeX / div, sizeY / div, format);
		div *= 2;
	} 

	//VIEWPORT
	myViewPort->TopLeftX = 0;
	myViewPort->TopLeftY = 0;
	myViewPort->Width = static_cast<float>(sizeX);
	myViewPort->Height = static_cast<float>(sizeY);
	myViewPort->MinDepth = 0.0f;
	myViewPort->MaxDepth = 1.0f;
	myContext->RSSetViewports(1, myViewPort);
	myContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

CU::Vec2Ui ISTE::DX11::GetBackBufferRes()
{
	return { myBackBuffer.GetWidth(), myBackBuffer.GetHeight() };
}
