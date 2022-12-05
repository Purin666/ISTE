#pragma once
#include <wrl/client.h> 
#include "ISTE/Graphics/RenderStateEnums.h"
using Microsoft::WRL::ComPtr;

struct ID3D11SamplerState;
struct ID3D11RasterizerState;
struct ID3D11BlendState;
struct ID3D11DepthStencilState;

namespace ISTE
{
	struct Context;

	class RenderStateManager
	{
	public:
		RenderStateManager();
		~RenderStateManager();

		bool Init();
		
		void SetRasterState(RasterizerState aState);
		void SetBlendState(BlendState aState);
		void SetSampleState(AdressMode aAM, SamplerState aSS, unsigned int aSlot = 0);	//0 = DefaultSampler
		void SetDepthState(DepthState aSamplerState, ReadWriteState aRoWState = ReadWriteState::eWrite);

	private:
		bool CreateRasterStates();
		bool CreateBlendStates();
		bool CreateSamplerStates();
		bool CreateDepthStates();

		Context* myCtx;

		ComPtr<ID3D11SamplerState>		mySamplerStates[(int)AdressMode::eCount][(int)SamplerState::eCount];
		ComPtr<ID3D11RasterizerState>	myRasterizerStates[(int)RasterizerState::eCount];
		ComPtr<ID3D11BlendState>		myBlendStates[(int)BlendState::eCount];
		ComPtr<ID3D11DepthStencilState> myDepthStencilStates[(int)DepthState::eCount][(int)ReadWriteState::eCount];
	};

}
