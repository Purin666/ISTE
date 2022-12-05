#pragma once
#include <vector>
#include <wrl/client.h> 

#include <ISTE/CU/MinHeap.hpp>
 
#include "ISTE/ECSB/System.h" 
#include "ISTE/Graphics/Resources/ShaderEnums.h"


namespace ISTE
{
	struct Context;

	class TransperancyDrawerSystem : public System
	{
	private:
		struct RenderCommand{
			bool operator<(RenderCommand aRight) { return myDistToCamera < aRight.myDistToCamera; };
			EntityID myEntity; 
			Shaders myShader;
			float myDistToCamera;
		};

	public:
		void Init();
		void PrepareRenderCommands();
		void Draw();

		void ClearCommands();

	private:
		CU::MinHeap<RenderCommand> myRenderCommands;
		Context* myCtx;
	};	
} 

