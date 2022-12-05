#pragma once
#include <wrl/client.h> 
#include "Shader.h" 

#include "ISTE/ECSB/ECSDefines.hpp"

using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;

namespace ISTE
{ 
	class VFXModelShader : public Shader
	{
	public:
		bool CreateBuffers() override;

		void Render(EntityID aEntity) override;
	private:
		const int PrepEntity(EntityID aEntity);
		void BindObjBuffer(const EntityID aEntity);
		ComPtr<ID3D11Buffer> myObjBuffer;
	};

}
