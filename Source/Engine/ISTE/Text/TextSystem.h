#pragma once
#include <wrl/client.h>
#include <vector>
#include "TextRenderCommand.h"
#include "ISTE/ECSB/System.h"
#include "ISTE/Text/TextRenderCommand.h"

using Microsoft::WRL::ComPtr;
class TextService;
struct ID3D11VertexShader;
struct ID3D11PixelShader; 
struct ID3D11InputLayout;
struct ID3D11Buffer;
namespace ISTE
{
	struct Context;

	class TextSystem : public System
	{
	public:
		TextSystem();
		~TextSystem();
		bool Init();

		void PrepareRenderCommands();
		void AddTextCommand(const TextRenderCommand&);
		void Draw();
		void ClearCommands();

	private:
		Context* myCtx;
		TextService* myTextService;

		int myQuadId;
		 
	private: //shader stuff
		bool InitShaders();
		bool CreateBuffers();

		void BindShader();
		void BindBuffers(TextRenderCommand& com);

		ComPtr<ID3D11Buffer> myObjBuffer;
		ComPtr<ID3D11InputLayout> myInputLayout;
		ComPtr<ID3D11VertexShader> myVertexShader;
		ComPtr<ID3D11PixelShader> myPixelShader;
	};
};
