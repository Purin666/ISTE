#pragma once
#include <wrl/client.h> 
#include "Shader.h" 
#include "ISTE/ECSB/ECSDefines.hpp"

using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;

namespace ISTE
{ 
	struct Sprite3DRenderCommand;
	
	class Sprite3DShader : public Shader
	{
	public:
		Sprite3DShader();
		~Sprite3DShader();

		bool CreateBuffers() override;
		
		void Render(EntityID aEntity) override;
	private:
		Sprite3DRenderCommand PrepEntity(EntityID aEntity);

		void BindObjBuffer(const ISTE::Sprite3DRenderCommand&);
		ComPtr<ID3D11Buffer> myObjBuffer;
		int mySpriteQuad;
	};

}