#pragma once
#include <wrl/client.h> 
#include <vector> 
#include <string>
#include <unordered_map>

#include "ISTE/ECSB/ECSDefines.hpp"
#include <ISTE/ECSB/System.h>
#include <ISTE/Graphics/ComponentAndSystem/ModelCommand.h>
#include <ISTE/Graphics/RenderDefines.h>

using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11InputLayout;

namespace ISTE
{


	class SceneHandler;
	class SystemManager;

	struct Context;

	class ModelDrawerSystem : public System
	{
	public:
		~ModelDrawerSystem();
		bool Init(const std::string& aVSPath, const std::string& aPSPath);
		void PrepareRenderCommands();
		void PrepareBatchedRenderCommands();
		void Draw();
		void DrawDepth();
		void DrawDepth(std::set<EntityID>&);
		void ClearCommands();

		//for out of system usage and testing 
		void AddRenderCommand(ModelCommand);

	private:
		Context* myCtx;
		SceneHandler* mySceneHandler;
		SystemManager* mySystemManager;
		std::vector<ModelCommand> myRenderCommands;
		std::unordered_map<uint64_t, ModelBatchValue> myBatchedRenderCommands;

	private: //shader stuff
		void BindObjBuffer(const ModelCommand&);
		void BindShader();

		bool InitShader(const std::string& aVSPath, const std::string& aPSPath);
		bool InitBuffers();

		ComPtr<ID3D11Buffer>		myVsObjBuffer;
		ComPtr<ID3D11Buffer>		myCommonGBuffer;
		ComPtr<ID3D11Buffer>		myInstanceBuffer;
		ComPtr<ID3D11InputLayout>	myInputLayout;
		ComPtr<ID3D11VertexShader>	myVertexShader;
		ComPtr<ID3D11PixelShader>	myPixelShader;
	};
}