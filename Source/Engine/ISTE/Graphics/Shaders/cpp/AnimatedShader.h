#pragma once 
#include <wrl/client.h> 
#include "Shader.h" 

#include <ISTE/Graphics/Resources/Model.h>
#include <ISTE/Graphics/Resources/Animation.h>
#include <ISTE/Graphics/ComponentAndSystem/AnimationCommand.h>

#include "ISTE/ECSB/ECSDefines.hpp"

using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;
namespace ISTE { 
	struct Animation;

	class AnimatedShader : public Shader
	{
	public:
		bool CreateBuffers() override;

		void Render(EntityID aEntity) override;
	private:
		void PrepEntity(EntityID aEntity, AnimationCommand& aOutRenderCommand);
		void BindObjBuffer(const AnimationCommand&);
		ComPtr<ID3D11Buffer> myObjBuffer;

		void UpdateAnimations(Animation::KeyFrame& aCurrentFrame, Animation::KeyFrame& aNextFrame, CU::Matrix4x4f& aParentsTransform,
			CU::Matrix4x4f* aFinalTransformList, CU::Matrix4x4f* aModelSpaceTransformList, CU::Matrix4x4f* aLocalSpaceTransformList,
			Model* aModel, int aBoneIndex, float delta);

	};
};
