#pragma once
#include <wrl/client.h>
#include <vector> 
#include <d3d11.h>

#include <ISTE/Math/Vec.h>

using Microsoft::WRL::ComPtr;
struct ID3D11Buffer; 

namespace ISTE { 
	struct VertexData {
		CU::Vec3f myPosition;	
		CU::Vec4f myColor;
		CU::Vec2f myTexCoord;	
		CU::Vec3f myNormal;		
		CU::Vec3f myBiNormal;	
		CU::Vec3f myTangent;
		CU::Vec4f myBones;
		CU::Vec4f myWeight;
	};


	struct Context;
	class Mesh
	{
	public:
		Mesh();
		~Mesh();

		void BindMesh();
		void BindIndexBuffer();
		inline unsigned int GetVertexCount() { return myVertexCount; }
		inline unsigned int GetIndexCount() { return myIndexCount; }
		inline ID3D11Buffer* GetVertexBuffer() { return myVertexBuffer; }

		bool Init();
		void SetVertexList(std::vector<VertexData> aVertexList) { myVertexList = aVertexList; }
		void SetIndexList(std::vector<unsigned int> aIndexList) { myIndexList = aIndexList; }

		std::vector<VertexData>&  GetVertexList() { return myVertexList; }
		std::vector<unsigned int>& GetIndexList() { return myIndexList; }

	private:
		friend class FBXLoader;
		friend class ModelManager;

		std::vector<unsigned int> myIndexList;
		std::vector<VertexData> myVertexList;
		Context* myCtx;
		ID3D11Buffer* myVertexBuffer;
		ID3D11Buffer* myIndexBuffer;

		unsigned int myVertexCount;
		unsigned int myIndexCount;
	};
}