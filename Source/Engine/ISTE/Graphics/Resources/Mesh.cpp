#include "Mesh.h"
#include <d3d11.h>

#include <ISTE/Context.h>
#include <ISTE/Graphics/DX11.h>
		  
ISTE::Mesh::Mesh()
{
	myVertexBuffer = nullptr;
	myIndexBuffer = nullptr;
	myIndexCount = (unsigned int) (-1);
	myVertexCount = (unsigned int)(-1);
	myCtx = nullptr;
} 

ISTE::Mesh::~Mesh()
{
	if(myVertexBuffer)
		myVertexBuffer->Release();

	if (myIndexBuffer)
		myIndexBuffer->Release();
}

void ISTE::Mesh::BindMesh()
{
	ID3D11DeviceContext* aDeviceContext = myCtx->myDX11->GetContext();
	unsigned int stride = sizeof(VertexData);
	unsigned int offset = 0;
	aDeviceContext->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
	aDeviceContext->IASetIndexBuffer(myIndexBuffer, DXGI_FORMAT_R32_UINT, 0); 
}

void ISTE::Mesh::BindIndexBuffer()
{ 
	myCtx->myDX11->GetContext()->IASetIndexBuffer(myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
}

bool ISTE::Mesh::Init()
{
	myCtx = ISTE::Context::Get();
	ID3D11Device* aDevice = myCtx->myDX11->GetDevice();
	myVertexCount = (unsigned int)myVertexList.size();
	myIndexCount = (unsigned int)myIndexList.size();

	HRESULT result;
	{
		D3D11_BUFFER_DESC vertexBufferDesc = {};
		vertexBufferDesc.ByteWidth = (unsigned int)(sizeof(VertexData) * myVertexList.size());
		vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA vertexData = { 0 };
		vertexData.pSysMem = &myVertexList[0];
		result = aDevice->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
		if (FAILED(result))
			return false;
	}

	{	
		D3D11_BUFFER_DESC indexBufferDesc = {};
		indexBufferDesc.ByteWidth = (unsigned int)(sizeof(unsigned int) * myIndexList.size());
		indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
		indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA indexData = { 0 };
		indexData.pSysMem = &myIndexList[0];

		result = aDevice->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
		if (FAILED(result))
			return false;
	}

	return true;
}
