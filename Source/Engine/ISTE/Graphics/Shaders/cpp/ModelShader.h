#pragma once
#include <wrl/client.h> 
#include "Shader.h" 

#include "ISTE/ECSB/ECSDefines.hpp"


using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;

namespace ISTE
{ 
	struct ModelCommand;
	class ModelShader : public Shader
	{
	public:
		bool CreateBuffers() override;

		void Render(EntityID aEntity) override;
	private:
		ModelCommand PrepEntity(EntityID aEntity);
		void BindObjBuffer(const ModelCommand&);
		ComPtr<ID3D11Buffer> myObjBuffer;
	};

}
