#pragma once
#include <vector>
#include <wrl/client.h> 

#include <ISTE/CU/MinHeap.hpp>
#include <ISTE/Math/Matrix4x4.h>

#include "ISTE/ECSB/System.h" 
#include "ISTE/Graphics/Resources/ShaderEnums.h"
#include "ISTE/Graphics/RenderStateEnums.h"


/*
	TODO
split up in to m


*/



using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;


namespace ISTE
{
	struct Context;

	class CustomDrawerSystem : public System 
	{
	public:
		struct RenderCommand {
			bool operator<(RenderCommand aRight) { return myDistToCamera > aRight.myDistToCamera; };
			EntityID myEntity;
			Shaders myShader;
			AdressMode myAdressMode;
			SamplerState mySamplerState;
			CU::Matrix4x4f myTransform;
			CU::Vec4f myColor;
			CU::Vec2f myUvChangeOverTime0;
			CU::Vec2f myUvChangeOverTime1;
			CU::Vec2f myUvChangeOverTime2;
			CU::Vec3f mySample0;
			CU::Vec3f mySample1;
			CU::Vec3f mySample2;
			float myElaspedTime;
			float myDuration;
			float myDistToCamera;
		};
		bool Init();
		void PrepareRenderCommands();
		void Draw();

	private:
		CU::MinHeap<RenderCommand> myRenderCommands;
		Context* myCtx;

	private:
		void BindObjBuffer(const RenderCommand&);

		ComPtr<ID3D11Buffer> myVsObjBuffer;
		ComPtr<ID3D11Buffer> myPsObjBuffer;
	}; 
}
