#include "DebugDrawer.h"
#include <d3d11.h>
#include "ISTE/Context.h"
#include "ISTE/Graphics/DX11.h"
#include "ISTE/Logger/Logger.h"

bool ISTE::DebugDrawer::Init()
{
	myCtx = Context::Get(); 
	if (!InitShaders())
		return false;
	if (!InitVertexBuffer())
		return false;

	return true;
}

void ISTE::DebugDrawer::AddStaticLineCommand(const LineCommand& aLC)
{
	myStaticLineCommands.emplace_back(aLC);
}

void ISTE::DebugDrawer::AddDynamicLineCommand(const LineCommand& aLC)
{
	myDynamicLineCommands.emplace_back(aLC);
}

void ISTE::DebugDrawer::ClearStaticCommands()
{
	myStaticLineCommands.clear();
}

void ISTE::DebugDrawer::ClearDynamicCommands()
{ 
	myDynamicLineCommands.clear();
}

void ISTE::DebugDrawer::Draw()
{
	DrawStaticLines();
	DrawDynamicLines();
}

void ISTE::DebugDrawer::DrawStaticLines()
{
	ID3D11DeviceContext* dCtx = myCtx->myDX11->GetContext();
	BindShader();

	D3D11_MAPPED_SUBRESOURCE bufferData;
	size_t size = myStaticLineCommands.size();
	size_t i = 0;
	while (i < size)
	{
		HRESULT res = dCtx->Map(myVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(res))
		{
			//line batching got fucked
			break;
		}
		size_t batchedLines = 0;
		LineVertex* vDataPtr = (LineVertex*)bufferData.pData;
		for (; i < size && batchedLines < LINE_DRAWER_INSTANCE_BATCH_SIZE / 2; i++)
		{
			LineCommand com = myStaticLineCommands[i];
			vDataPtr[batchedLines].myColor			= com.myColor;
			vDataPtr[batchedLines].myPosition		= com.myFromPosition;
			vDataPtr[batchedLines + 1].myColor		= com.myColor;
			vDataPtr[batchedLines + 1].myPosition	= com.myToPosition;
			batchedLines += 2;
			Logger::LoggBatchedDrawCall();
		}

		dCtx->Unmap(myVertexBuffer.Get(), 0);
		Logger::LoggDrawCall();
		dCtx->Draw(batchedLines, 0);
	}
}

void ISTE::DebugDrawer::DrawDynamicLines()
{

	ID3D11DeviceContext* dCtx = myCtx->myDX11->GetContext();
	BindShader();

	D3D11_MAPPED_SUBRESOURCE bufferData;
	size_t size = myDynamicLineCommands.size();
	size_t i = 0;
	while (i < size)
	{
		HRESULT res = dCtx->Map(myVertexBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &bufferData);
		if (FAILED(res))
		{
			//line batching got fucked
			break;
		}
		size_t batchedLines = 0;
		LineVertex* vDataPtr = (LineVertex*)bufferData.pData;
		for (; i < size && batchedLines < LINE_DRAWER_INSTANCE_BATCH_SIZE / 2; i++)
		{
			LineCommand com = myDynamicLineCommands[i];
			vDataPtr[batchedLines].myColor = com.myColor;
			vDataPtr[batchedLines].myPosition = com.myFromPosition;
			vDataPtr[batchedLines + 1].myColor = com.myColor;
			vDataPtr[batchedLines + 1].myPosition = com.myToPosition;
			batchedLines += 2;
			Logger::LoggBatchedDrawCall();
		}

		dCtx->Unmap(myVertexBuffer.Get(), 0);
		Logger::LoggDrawCall();
		dCtx->Draw(batchedLines, 0);
	}
}


void ISTE::DebugDrawer::BindShader()
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();
	
	unsigned int stride = sizeof(LineVertex);
	unsigned int offset = 0;
	context->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), &stride, &offset);

	context->VSSetShader(myVertexShader.Get(), 0, 0);
	context->PSSetShader(myPixelShader.Get(), 0, 0);
	context->IASetInputLayout(myInputLayout.Get());
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
}

void ISTE::DebugDrawer::BindBuffer(const LineCommand& com)
{
	ID3D11DeviceContext* dCtx = myCtx->myDX11->GetContext(); 
}

bool ISTE::DebugDrawer::InitShaders()
{
	ID3D11Device* aDevice = myCtx->myDX11->GetDevice();
	HRESULT result;
	std::string vsData;
	{ 
	if (!ISTE::DX11::CreateVertexShader("Shaders/Line_VS.cso", myVertexShader.GetAddressOf(), vsData))
		return false;
	if(!ISTE::DX11::CreatePixelShader("Shaders/Line_PS.cso", myPixelShader.GetAddressOf())) 
		return false;
	}

	{
	D3D11_INPUT_ELEMENT_DESC layout[] = {
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			}

		};

	int elementCount = sizeof(layout) / sizeof(layout[0]);
	result = aDevice->CreateInputLayout(layout, elementCount, vsData.data(), vsData.size(), myInputLayout.GetAddressOf());
	if (FAILED(result))
		return false;
	}
	return true;
}

bool ISTE::DebugDrawer::InitVertexBuffer()
{
	ID3D11Device* dCtx = myCtx->myDX11->GetDevice();
	
	//obj cbuffer
	{
	D3D11_BUFFER_DESC bufferDesc = {};
	bufferDesc.ByteWidth		= (unsigned int)(sizeof(LineVertex) * LINE_DRAWER_INSTANCE_BATCH_SIZE);
	bufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
	
	HRESULT res = dCtx->CreateBuffer(&bufferDesc, 0, myVertexBuffer.GetAddressOf());
	if (FAILED(res))
		return false;
	}
	
	////instance buffer
	//{
	//	D3D11_BUFFER_DESC bufferDesc = {};
	//bufferDesc.ByteWidth		= (unsigned int)sizeof(LineVertex) * 2.f;
	//bufferDesc.BindFlags		= D3D11_BIND_VERTEX_BUFFER;
	//bufferDesc.Usage			= D3D11_USAGE_DYNAMIC;
	//bufferDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
	//}


	return true;
}
 