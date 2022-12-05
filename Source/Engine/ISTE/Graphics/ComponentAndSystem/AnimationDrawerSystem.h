#pragma once
#include <wrl/client.h> 
#include <string>
#include <vector>
#include <set>

#include <ISTE/ECSB/System.h>
#include "ISTE/ECSB/ECSDefines.hpp"
#include <ISTE/CU/HashMap.hpp>
#include <ISTE/Math/Matrix4x4.h> 
#include <ISTE/Graphics/ComponentAndSystem/AnimationCommand.h>
#include <ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/AnimationBlendComponent.h>
#include <ISTE/Graphics/RenderDefines.h>
#include <ISTE/Graphics/Resources/Model.h>
#include <ISTE/Graphics/Resources/Animation.h> 

/*
We can potentially reserv the rendercommands and overwrite the data
ie never clearing anything 

itll just take a lot of wasted memory 
*/



using Microsoft::WRL::ComPtr;
struct ID3D11Buffer;
struct ID3D11PixelShader;
struct ID3D11VertexShader;
struct ID3D11InputLayout;

struct aiScene;
struct aiNode;
namespace ISTE {
	struct Context;
	struct Animation;
	struct AnimatorComponent; 
	class AnimationDrawerSystem : public System
	{
	public:
		AnimationDrawerSystem();
		~AnimationDrawerSystem();
		bool Init(std::string, std::string); 

		inline void PauseAnimations() { myPlayFactor = 0.f; }
		inline void ResumeAnimations() { myPlayFactor = 1.f; }

		void PrepareRenderCommands();
		void Draw();
		void DrawVisualization();
		void DrawDepth();
		void DrawDepth(std::set<EntityID>&);
		void ClearCommands();

		ComponentMask GetSingnature(size_t aComplexity) { return mySingnatures[aComplexity]; }
		ComponentMask GetIgnoreMask(size_t aComplexity) { return myIgnoreIfMasks[aComplexity]; }

		static void CalcCurrentKeyFrameAndDelta(AnimationID aAnimID, float aTimer, Animation::KeyFrame& aOutCurrentFrame, Animation::KeyFrame& aOutNextFrame, float& aOutDelta);
		static void UpdateLocaltransforms(ModelID aModelID, Pose& aPose, Animation::KeyFrame& aCurrentFrame, Animation::KeyFrame& aNextFrame, float aDelta);
		static void UpdateLocaltransforms(Model* aCurrentModel, Pose& aPose, Animation::KeyFrame& aCurrentFrame, Animation::KeyFrame& aNextFrame, float aDelta);
		static void UpdateModeltransforms(ModelID aModelID, Pose& aPose, CU::Matrix4x4f aParentTransform, int aBoneIndex);
		static void UpdateModeltransforms(const Bone* aModelBoneList, Pose& aPose, CU::Matrix4x4f aParentTransform, int aBoneIndex);
		static void ApplyBindSpaceInverse(Bone* aModelBoneList, size_t aSize, Pose& aPose, CU::Matrix4x4f*& aOutBindPoseList);
		static void ApplyBindSpaceInverse(Model*, Pose& aPose, CU::Matrix4x4f* aOutBindPoseList);
		
		static void UpdatePartialAnimLocalTransforms(
			const Pose& AnimInA, const Pose& AnimInB, Pose& aPoseOut,
			Model*, float aDelta, int aBoneIndx, 
			int boneIDToPartialFrom = 0, float PartialStrength = 0
		);
	private:
		//does the whole BindPosePass in the same func
		void UpdateAnimations(Animation::KeyFrame& aCurrentFrame, Animation::KeyFrame& aNextFrame, CU::Matrix4x4f& aParentsTransform,
			CU::Matrix4x4f* aFinalTransformList, CU::Matrix4x4f* aModelSpaceTransformList, CU::Matrix4x4f* aLocalSpaceTransformList,
			Model* aModel, int aBoneIndex, float delta);

		void CreateSkeletonVisualisation(const CU::Matrix4x4f& aModelTransform, const CU::Matrix4x4f& aBoneF,
			ISTE::AnimatorComponent* aAnimationThing, ISTE::Model* aModel, int aBoneIndex);

		float PerformeInterpolationUGLY(AnimationBlendData& data, InterpolationID id);

		void PrepareSimpelAnimations();
		void PrepareBlendedAnimations();

		Context* myCtx;

		std::vector<AnimationCommand> myRenderCommands; 

		void (AnimationDrawerSystem::*BlendOperation[(int)AnimBlendOperationType::eCount])(AnimatorComponent* anAnimator, AnimationBlendComponent*, const AnimationBlendNode, Pose&);
		void InterpolateAnimation	(AnimatorComponent*, AnimationBlendComponent*, const AnimationBlendNode, Pose&);
		void PartialAnimation		(AnimatorComponent*, AnimationBlendComponent*, const AnimationBlendNode, Pose&);
		void FetchAnimation			(AnimatorComponent*, AnimationBlendComponent*, const AnimationBlendNode, Pose&);

	private: //shader stuff
		bool InitBuffers();
		bool InitShaders(std::string, std::string);
		void BindBuffer(AnimationCommand&);
		void BindShader();

		ComPtr<ID3D11Buffer> myObjBuffer;
		ComPtr<ID3D11Buffer> myCommonGBuffer;
		ComPtr<ID3D11InputLayout> myInputLayout;
		ComPtr<ID3D11VertexShader> myVertexShader;
		ComPtr<ID3D11PixelShader> myPixelShader;

		float myPlayFactor = 1.f; // multiplied with timeDelta, used to "pause" animations; when 1.f animations play; when 0.f animations are "paused"
	};

};
