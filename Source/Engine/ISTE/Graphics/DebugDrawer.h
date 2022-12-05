#pragma once
#include <wrl/client.h>
#include <vector>
#include "ISTE/Math/Vec.h"
#include "ISTE/Graphics/RenderDefines.h"
using Microsoft::WRL::ComPtr;

struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;

namespace ISTE {

	struct Context;
	struct LineCommand{
		CU::Vec3f myFromPosition;
		CU::Vec3f myToPosition;
		CU::Vec3f myColor;

		//size is something that could be added
	};
	
	class DebugDrawer
	{
	private: //structs
		struct LineVertex
		{
			CU::Vec3f myPosition; 
			CU::Vec3f myColor;
		};

	public:
		DebugDrawer() = default;
		~DebugDrawer() = default;

		bool Init();

		void AddStaticLineCommand(const LineCommand&);
		void AddDynamicLineCommand(const LineCommand&);
		void ClearStaticCommands();
		void ClearDynamicCommands();

		void Draw();

	private:
		void DrawStaticLines();
		void DrawDynamicLines();

		std::vector <LineCommand> myStaticLineCommands;
		std::vector <LineCommand> myDynamicLineCommands;
		
		Context* myCtx;
	
	private: //Shader stuff
		void BindShader();
		void BindBuffer(const LineCommand&);
		
		bool InitShaders();
		bool InitVertexBuffer();

		ComPtr<ID3D11PixelShader>		myPixelShader;
		ComPtr<ID3D11VertexShader>		myVertexShader;
		ComPtr<ID3D11InputLayout>		myInputLayout;
		ComPtr<ID3D11Buffer>			myVertexBuffer;
	};
}