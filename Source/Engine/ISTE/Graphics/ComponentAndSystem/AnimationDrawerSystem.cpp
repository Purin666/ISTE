#include "AnimationDrawerSystem.h"
#include <d3d11.h>
#include <fstream>
#include <pix.h>

#include <ISTE/Context.h>
#include <ISTE/Graphics/DX11.h>
#include <ISTE/Graphics/GraphicsEngine.h>
#include <ISTE/Logger/Logger.h>
#include <ISTE/Time/TimeHandler.h>
#include <ISTE/Graphics/Resources/TextureManager.h>
#include <ISTE/Graphics/Resources/ModelManager.h> 
#include <ISTE/Graphics/Resources/AnimationManager.h> 
#include <ISTE/Graphics/Resources/Model.h>
#include <ISTE/Scene/Scene.h> 
#include <ISTE/Scene/SceneHandler.h>  
#include <ISTE/Graphics/DebugDrawer.h>

//add AnimationComponent
#include <ISTE/Graphics/ComponentAndSystem/MaterialComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/TransformComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/ModelComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/CustomShaderComponent.h>
#include <ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h>
#include <ISTE/Graphics/RenderStateManager.h>

#include <ISTE/Math/Quaternion.h>
#include <ISTE/Math/Matrix4x4.h>
#include <ISTE/Math/Vec3.h>
#include <ISTE/CU/HashMap.hpp> 
struct ObjectBuffer {
	CU::Matrix4x4f myTransform;
	CU::Matrix4x4f myPrevTransform;
	CU::Matrix4x4f myBoneTransform[MAX_BONE_COUNT];
};

struct CommonGBuffer {
	CU::Vec3f	myColor;
	EntityIndex	myIndex;
	float		myRenderFlags;
	CU::Vec3f	myGHarb;
};

ISTE::AnimationDrawerSystem::AnimationDrawerSystem()
{
}
ISTE::AnimationDrawerSystem::~AnimationDrawerSystem()
{
}


void ISTE::AnimationDrawerSystem::FetchAnimation(const AnimatorComponent* anAnimator, AnimationBlendComponent* aCompData, const AnimationBlendNode aNode, Pose& outPose)
{
	float delta;
	Animation::KeyFrame curr, next;
	
	FetchAnimationData& fetchData	= aCompData->myAnimationBlendData.myFetchOperations[aNode.myDataIndex]; 
	Animation*			anim		= myCtx->myAnimationManager->GetAnimation(fetchData.myAnimation);
	ModelID				modelID		= myCtx->myAnimationManager->GetAnimation(fetchData.myAnimation)->myModel;

	fetchData.myTimer += myCtx->myTimeHandler->GetDeltaTime() * anAnimator->mySpeedModifier * myPlayFactor;
	if (fetchData.myTimer > anim->myLengthInSeconds) 
		fetchData.myTimer = 0;

	CalcCurrentKeyFrameAndDelta(fetchData.myAnimation, fetchData.myTimer, curr, next, delta);
	UpdateLocaltransforms(modelID, outPose, curr, next, delta);
}

void ISTE::AnimationDrawerSystem::InterpolateAnimation(const AnimatorComponent* anAnimator, AnimationBlendComponent* aCompData, const AnimationBlendNode aNode, Pose& outPose)
{
	Pose			animA; 
	CU::Vec3f		scaleA;
	CU::Vec3f		scaleB;
	CU::Quaternionf quatA;
	CU::Quaternionf quatB;
	CU::Vec3f		translationA;
	CU::Vec3f		translationB;
	CU::Matrix4x4f	mA, mB, mS, mT;

	ModelID			modelID = myCtx->myAnimationManager->GetAnimation(anAnimator->myCurrentAnimation)->myModel;
	size_t			boneCount = myCtx->myModelManager->GetBoneNameToIdMap(modelID).size();
	const AnimationInterpolationData& interpData = aCompData->myAnimationBlendData.myInterpOperations[aNode.myDataIndex];
	
	
	float lerpValue = PerformeInterpolationUGLY(aCompData->myAnimationBlendData, interpData.myInterpID);
	
	(*this.*BlendOperation[(int)interpData.myNodes[0].myType])(anAnimator,aCompData, interpData.myNodes[0], animA);
	(*this.*BlendOperation[(int)interpData.myNodes[1].myType])(anAnimator,aCompData, interpData.myNodes[1], outPose);


	for (size_t i = 0; i < boneCount; i++)
	{ 
		mA = animA.myLocalSpaceBoneTransforms[i];
		mB = outPose.myLocalSpaceBoneTransforms[i];

		scaleA			= mA.DecomposeScale();
		quatA			= CU::Quaternionf(mA);
		translationA	= mA.GetTranslationV3();
		
		scaleB			= mB.DecomposeScale();
		quatB			= CU::Quaternionf(mB);
		translationB	= mB.GetTranslationV3();
		 
		CU::Vec3f		finalS = CU::Vec3f::Lerp		(scaleA,		scaleB,			lerpValue); 
		CU::Quaternionf finalR = CU::Quaternionf::Slerp (quatA,			quatB,			lerpValue); 
		CU::Vec3f		finalT = CU::Vec3f::Lerp		(translationA,	translationB,	lerpValue);
	
	
		mS(1, 1) = finalS.x;
		mS(2, 2) = finalS.y;
		mS(3, 3) = finalS.z;
		mT.GetTranslationV3() = finalT;
		outPose.myLocalSpaceBoneTransforms[i] = mS * finalR.GetRotationMatrix4X4() * mT;
	}  
	 
}


void ISTE::AnimationDrawerSystem::PartialAnimation(const AnimatorComponent* anAnimator, AnimationBlendComponent* aCompData, const AnimationBlendNode aNode, Pose& outPose)
{
	Pose			animA;
	ModelID			modelID		= myCtx->myAnimationManager->GetAnimation(anAnimator->myCurrentAnimation)->myModel;
	Model*			model		= myCtx->myModelManager->GetModel(modelID);
	size_t			boneCount	= model->myBoneNameToId.size();

	const AnimationPartialData& interpData = aCompData->myAnimationBlendData.myPartialOperationData[aNode.myDataIndex];


	float lerpValue = aCompData->myAnimationBlendData.myPartialOperationData[aNode.myDataIndex].myPartialInfluence;
	int myJointID = aCompData->myAnimationBlendData.myPartialOperationData[aNode.myDataIndex].myDeviderID;

	(*this.*BlendOperation[(int)interpData.myNodes[0].myType])(anAnimator, aCompData, interpData.myNodes[0], animA);
	(*this.*BlendOperation[(int)interpData.myNodes[1].myType])(anAnimator, aCompData, interpData.myNodes[1], outPose);

	UpdatePartialAnimLocalTransforms(animA, outPose, outPose, model, lerpValue, 0, myJointID);
}

bool ISTE::AnimationDrawerSystem::Init(std::string vs, std::string ps)
{
	myCtx = Context::Get();
	if (!InitBuffers())
		return false;
	if (!InitShaders(vs,ps))
		return false;

	BlendOperation[(int)AnimBlendOperationType::ePartial]			= &AnimationDrawerSystem::PartialAnimation;
	BlendOperation[(int)AnimBlendOperationType::eInterpolate]		= &AnimationDrawerSystem::InterpolateAnimation;
	BlendOperation[(int)AnimBlendOperationType::eFetchAnimation]	= &AnimationDrawerSystem::FetchAnimation;
	
	return true;
}


void ISTE::AnimationDrawerSystem::PrepareRenderCommands()
{ 
	PrepareSimpelAnimations();
	PrepareBlendedAnimations(); 
} 

void ISTE::AnimationDrawerSystem::PrepareSimpelAnimations()
{
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& modelPool = scene.GetComponentPool<ModelComponent>();
	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
	ComponentPool& materialPool = scene.GetComponentPool<MaterialComponent>();
	ComponentPool& animatorPool = scene.GetComponentPool<AnimatorComponent>();


	auto& models = myCtx->myModelManager->GetModelList();
	auto& animations = myCtx->myAnimationManager->GetAnimationList();

	AnimationCommand command;
	 
	for (EntityID entity : myEntities[0])
	{
		{
			MaterialComponent* materialComp = (MaterialComponent*)materialPool.Get(GetEntityIndex(entity));
			command.myRenderFlags = materialComp->myRenderFlags;
			memcpy(command.myTextureIDs, materialComp->myTextures, sizeof(TextureID) * MAX_MATERIAL_COUNT * MAX_MESH_COUNT);
		}

		//processes model data
		{
			ModelComponent* modelData = (ModelComponent*)modelPool.Get(GetEntityIndex(entity));
			if (modelData->myModelId == ModelID(-1))
				continue;
			command.myModelId = modelData->myModelId;
			command.myColor = { modelData->myColor.x,modelData->myColor.y,modelData->myColor.z,1 };
			command.myUV = modelData->myUV;
			command.myUVScale = modelData->myUVScale;
			command.mySamplerState = modelData->mySamplerState;
			command.myAdressMode = modelData->myAdressMode;
			command.myEntityIndex = GetEntityIndex(entity);
			//oomfie... does a safety check in case it should be here at all
			//99.99% of the time its false so it should be fine... r-i-ight
			if (myCtx->myModelManager->GetModelType(modelData->myModelId) == ModelType::eTransparantModel)
			{
				auto* comp = scene.AssignComponent<CustomShaderComponent>(entity);
				comp->myShader = Shaders::eDefaultModelShader;
			}
		}

		//processses transform data
		{
			TransformComponent* transformData = (TransformComponent*)transformPool.Get(GetEntityIndex(entity));
			command.myTransform = transformData->myCachedTransform; 
			command.myPrevTransform; //= myPrevRenderCommands[counter].myTransform;
		}

		//updates animation
		{
			AnimatorComponent* animatorComp = (AnimatorComponent*)animatorPool.Get(GetEntityIndex(entity));
			if (animatorComp->myCurrentAnimation == AnimationID(-1))
			{
				if (animatorComp->myAnimations[0] == AnimationID(-1))
					continue; // this was return........
				animatorComp->myCurrentAnimation = animatorComp->myAnimations[0];
			}

			Animation* currentAnimation = animations.GetByIterator(animatorComp->myCurrentAnimation);
			if (animatorComp->myAnimationState == AnimationState::ePlaying)
				animatorComp->myTimer += myCtx->myTimeHandler->GetDeltaTime() * animatorComp->mySpeedModifier * myPlayFactor;

			if (animatorComp->myTimer > currentAnimation->myLengthInSeconds)
			{
				if (!animatorComp->myLoopingFlag)
				{
					animatorComp->myAnimationState = AnimationState::eEnded;
					animatorComp->myTimer = currentAnimation->myLengthInSeconds;
				}
				else
					animatorComp->myTimer = 0;

			}

			float durationInFrames = animatorComp->myTimer * currentAnimation->myFps;
			int currentFrame = (int)std::floor(durationInFrames);
			int nextFrame = currentFrame + 1;
			float delta = durationInFrames - (float)currentFrame;

			Model* model = models.GetByIterator(command.myModelId);
			command.myMeshCount = model->myMeshCount; 

			UpdateAnimations(currentAnimation->myKeyFrames[currentFrame], currentAnimation->myKeyFrames[nextFrame], CU::Matrix4x4f(), command.myBindPoseInverse,
				animatorComp->myPose.myModelSpaceBoneTransforms, animatorComp->myPose.myLocalSpaceBoneTransforms, model, 0, delta);

			memcpy(animatorComp->myPose.myBindSpaceTransforms, command.myBindPoseInverse, sizeof(CU::Matrix4x4f) * MAX_BONE_COUNT);
		}
		myRenderCommands.push_back(command); 
	}
}

void ISTE::AnimationDrawerSystem::PrepareBlendedAnimations()
{
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& modelPool = scene.GetComponentPool<ModelComponent>();
	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
	ComponentPool& materialPool = scene.GetComponentPool<MaterialComponent>();
	ComponentPool& animatorPool = scene.GetComponentPool<AnimatorComponent>();
	ComponentPool& blendPool = scene.GetComponentPool<AnimationBlendComponent>();

	auto& models = myCtx->myModelManager->GetModelList();
	auto& animations = myCtx->myAnimationManager->GetAnimationList();

	AnimationCommand command;
	size_t counter = 0;
	 
	for (EntityID entity : myEntities[1])
	{
		{
			MaterialComponent* materialComp = (MaterialComponent*)materialPool.Get(GetEntityIndex(entity));
			command.myRenderFlags = materialComp->myRenderFlags;
			memcpy(command.myTextureIDs, materialComp->myTextures, sizeof(TextureID) * MAX_MATERIAL_COUNT * MAX_MESH_COUNT);
		}

		//processes model data
		{
			ModelComponent* modelData = (ModelComponent*)modelPool.Get(GetEntityIndex(entity));
			if (modelData->myModelId == ModelID(-1))
				continue;
			command.myModelId = modelData->myModelId;
			command.myColor = { modelData->myColor.x,modelData->myColor.y,modelData->myColor.z,1 };
			command.myUV = modelData->myUV;
			command.myUVScale = modelData->myUVScale;
			command.mySamplerState = modelData->mySamplerState;
			command.myAdressMode = modelData->myAdressMode;
			command.myEntityIndex = GetEntityIndex(entity);
			//oomfie... does a safety check in case it should be here at all
			//99.99% of the time its false so it should be fine... r-i-ight
			if (myCtx->myModelManager->GetModelType(modelData->myModelId) == ModelType::eTransparantModel)
			{
				auto* comp = scene.AssignComponent<CustomShaderComponent>(entity);
				comp->myShader = Shaders::eDefaultModelShader;
			}
		}

		//processses transform data
		{
			TransformComponent* transformData = (TransformComponent*)transformPool.Get(GetEntityIndex(entity));
			command.myTransform = transformData->myCachedTransform; 
		}

		//updates animation
		{
			AnimatorComponent* animatorComp = (AnimatorComponent*)animatorPool.Get(GetEntityIndex(entity));
			AnimationBlendComponent* blendComp = (AnimationBlendComponent*)blendPool.Get(GetEntityIndex(entity));
			if (animatorComp->myCurrentAnimation == AnimationID(-1))
				continue;

			AnimationBlendNode& rootNode = blendComp->myAnimationBlendData.myRootNode;
			(*this.*BlendOperation[(int)rootNode.myType])(animatorComp, blendComp, rootNode, animatorComp->myPose);
			 
			Model* model = myCtx->myModelManager->GetModel(command.myModelId);
			command.myMeshCount = model->myMeshCount;

			UpdateModeltransforms(model->myIndex, animatorComp->myPose, CU::Matrix4x4f(), 0);
			ApplyBindSpaceInverse(model, animatorComp->myPose, command.myBindPoseInverse);
		}
		myRenderCommands.push_back(command);
		counter++;
	}
}



void ISTE::AnimationDrawerSystem::CalcCurrentKeyFrameAndDelta(AnimationID aAnimID, float aTimer, Animation::KeyFrame& aOutCurrentFrame, Animation::KeyFrame& aOutNextFrame, float& aOutDelta)
{ 
	//if (animatorComp->myAnimationState == AnimationState::ePlaying)
	//	animatorComp->myTimer += Context::Get()->myTimeHandler->GetDeltaTime();

	//if (animatorComp->myTimer > currentAnimation->myLengthInSeconds)
	//{
	//	if (!animatorComp->myLoopingFlag)
	//	{
	//		animatorComp->myAnimationState = AnimationState::eEnded;
	//	}
	//	animatorComp->myTimer = 0;
	//}

	Animation* currentAnimation = Context::Get()->myAnimationManager->GetAnimation(aAnimID);
	float durationInFrames = aTimer * currentAnimation->myFps;
	int currentFrame = (int)std::floor(durationInFrames);
	int nextFrame = currentFrame + 1;
	float delta = durationInFrames - (float)currentFrame;
	aOutCurrentFrame	= currentAnimation->myKeyFrames[currentFrame];
	aOutNextFrame		= currentAnimation->myKeyFrames[nextFrame];
	aOutDelta			= delta;
}

void ISTE::AnimationDrawerSystem::UpdateLocaltransforms(Model* aCurrentModel, Pose& aPose, Animation::KeyFrame& aCurrentFrame, Animation::KeyFrame& aNextFrame, float aDelta)
{  
	CU::Matrix4x4f boneScaleTransform; 
	CU::Matrix4x4f bonePosTransform;
	for (size_t boneIndex = 0; boneIndex < aCurrentModel->myBoneCount; boneIndex++)
	{
		
		// Interpolate between the frames
		CU::Vec3f		T = CU::Vec3f::Lerp(aCurrentFrame.myTransforms[boneIndex].myPosition,			aNextFrame.myTransforms[boneIndex].myPosition, aDelta);
		CU::Quaternionf R = CU::Quaternionf::Slerp(aCurrentFrame.myTransforms[boneIndex].myQuaternion,	aNextFrame.myTransforms[boneIndex].myQuaternion, aDelta);
		CU::Vec3f		S = CU::Vec3f::Lerp(aCurrentFrame.myTransforms[boneIndex].myScale,				aNextFrame.myTransforms[boneIndex].myScale, aDelta);

		//scale
		boneScaleTransform(1, 1) = S.x;
		boneScaleTransform(2, 2) = S.y;
		boneScaleTransform(3, 3) = S.z; 

		//translation
		bonePosTransform(4, 1) = T.x;
		bonePosTransform(4, 2) = T.y;
		bonePosTransform(4, 3) = T.z; 

		aPose.myLocalSpaceBoneTransforms[boneIndex] = boneScaleTransform * R.GetRotationMatrix4X4() * bonePosTransform;
	}
}
void ISTE::AnimationDrawerSystem::UpdatePartialAnimLocalTransforms(
	const Pose& AnimInA, const Pose& AnimInB, Pose& aPoseOut,
	Model* aCurrentModel, float aDelta, int aBoneIndx, 
	int boneIDToPartialFrom, float PartialStrength)
{
	Bone* bone = &aCurrentModel->myBones[aBoneIndx];

	if (aBoneIndx == boneIDToPartialFrom)
		PartialStrength = aDelta;
	for (size_t childIndx = 0; childIndx < aCurrentModel->myBones[aBoneIndx].myChildrenCount; childIndx++)
	{
		UpdatePartialAnimLocalTransforms(AnimInA, AnimInB, aPoseOut, 
			aCurrentModel,aDelta, bone->myChildren[childIndx], boneIDToPartialFrom, PartialStrength);
	}

	CU::Vec3f		scaleA;
	CU::Vec3f		scaleB;
	CU::Quaternionf quatA;
	CU::Quaternionf quatB;
	CU::Vec3f		translationA;
	CU::Vec3f		translationB;
	CU::Matrix4x4f	mA, mB, mS, mT;

	mA = AnimInA.myLocalSpaceBoneTransforms[aBoneIndx];
	mB = AnimInB.myLocalSpaceBoneTransforms[aBoneIndx];
	
	scaleA = mA.DecomposeScale();
	quatA = CU::Quaternionf(mA);
	translationA = mA.GetTranslationV3();

	scaleB = mB.DecomposeScale();
	quatB = CU::Quaternionf(mB);
	translationB = mB.GetTranslationV3();

	CU::Vec3f		finalS = CU::Vec3f::Lerp(scaleA, scaleB, PartialStrength);
	CU::Quaternionf finalR = CU::Quaternionf::Slerp(quatA, quatB, PartialStrength);
	CU::Vec3f		finalT = CU::Vec3f::Lerp(translationA, translationB, PartialStrength);

	mS(1, 1) = finalS.x;
	mS(2, 2) = finalS.y;
	mS(3, 3) = finalS.z;
	mT.GetTranslationV3() = finalT;
	aPoseOut.myLocalSpaceBoneTransforms[aBoneIndx] = mS * finalR.GetRotationMatrix4X4() * mT;
}
void ISTE::AnimationDrawerSystem::UpdateLocaltransforms(ModelID aCurrentModelId, Pose& aPose, Animation::KeyFrame& aCurrentFrame, Animation::KeyFrame& aNextFrame, float aDelta)
{
	CU::Matrix4x4f boneScaleTransform;
	CU::Matrix4x4f bonePosTransform;
	Model* aCurrentModel = Context::Get()->myModelManager->GetModel(aCurrentModelId);

	for (size_t boneIndex = 0; boneIndex < aCurrentModel->myBoneCount; boneIndex++)
	{

		// Interpolate between the frames
		CU::Vec3f		T = CU::Vec3f::Lerp(aCurrentFrame.myTransforms[boneIndex].myPosition, aNextFrame.myTransforms[boneIndex].myPosition, aDelta);
		CU::Quaternionf R = CU::Quaternionf::Slerp(aCurrentFrame.myTransforms[boneIndex].myQuaternion, aNextFrame.myTransforms[boneIndex].myQuaternion, aDelta);
		CU::Vec3f		S = CU::Vec3f::Lerp(aCurrentFrame.myTransforms[boneIndex].myScale, aNextFrame.myTransforms[boneIndex].myScale, aDelta);

		//scale
		boneScaleTransform(1, 1) = S.x;
		boneScaleTransform(2, 2) = S.y;
		boneScaleTransform(3, 3) = S.z;

		//translation
		bonePosTransform(4, 1) = T.x;
		bonePosTransform(4, 2) = T.y;
		bonePosTransform(4, 3) = T.z;

		aPose.myLocalSpaceBoneTransforms[boneIndex] = boneScaleTransform * R.GetRotationMatrix4X4() * bonePosTransform;
	}
}

void ISTE::AnimationDrawerSystem::UpdateModeltransforms(ModelID aModelID, Pose& aPose, CU::Matrix4x4f aParentTransform, int aBoneIndex)
{
	Bone currentJoint = Context::Get()->myModelManager->GetModel(aModelID)->myBones[aBoneIndex];

	aPose.myModelSpaceBoneTransforms[aBoneIndex] = aPose.myLocalSpaceBoneTransforms[aBoneIndex] * aParentTransform;
	for (size_t i = 0; i < currentJoint.myChildrenCount; i++)
	{
		UpdateModeltransforms(aModelID, aPose, aPose.myModelSpaceBoneTransforms[aBoneIndex], currentJoint.myChildren[i]);
	}
}

void ISTE::AnimationDrawerSystem::UpdateModeltransforms(const Bone* aModelBoneList, Pose& aPose, CU::Matrix4x4f aParentTransform, int aBoneIndex)
{
	Bone currentJoint = aModelBoneList[aBoneIndex];

	aPose.myModelSpaceBoneTransforms[aBoneIndex] = aPose.myLocalSpaceBoneTransforms[aBoneIndex] * aParentTransform;
	for (size_t i = 0; i < currentJoint.myChildrenCount; i++)
	{
		UpdateModeltransforms(aModelBoneList, aPose, aPose.myModelSpaceBoneTransforms[aBoneIndex], currentJoint.myChildren[i]);
	}
}

void ISTE::AnimationDrawerSystem::ApplyBindSpaceInverse(Bone* aModelBoneList, size_t aSize, Pose& aPose, CU::Matrix4x4f*& aOutBindPoseList)
{
	for (size_t i = 0; i < aSize; i++)
	{
		aOutBindPoseList[i] = aModelBoneList[i].myBindToBone * aPose.myModelSpaceBoneTransforms[i];
	}
}

void ISTE::AnimationDrawerSystem::ApplyBindSpaceInverse(Model* aModel, Pose& aPose, CU::Matrix4x4f* aOutBindPoseList)
{
	size_t size = aModel->myBoneNameToId.size();
	for (size_t i = 0; i < size; i++)
	{
		aOutBindPoseList[i] = aModel->myBones[i].myBindToBone * aPose.myModelSpaceBoneTransforms[i];
	}
}
 
void ISTE::AnimationDrawerSystem::UpdateAnimations(Animation::KeyFrame& aCurrentFrame, Animation::KeyFrame& aNextFrame, CU::Matrix4x4f& aParentsTransform,
	CU::Matrix4x4f* aFinalTransformList, CU::Matrix4x4f* aModelSpaceTransformList, CU::Matrix4x4f* aLocalSpaceTransformList,
	Model* aModel, int aBoneIndex, float delta)
{

	CU::Matrix4x4f boneLocalSpaceTransform;
	{
		// Interpolate between the frames
		CU::Vec3f T = CU::Vec3f::Lerp(aCurrentFrame.myTransforms[aBoneIndex].myPosition, aNextFrame.myTransforms[aBoneIndex].myPosition, delta);
		CU::Quaternionf R = CU::Quaternionf::Slerp(aCurrentFrame.myTransforms[aBoneIndex].myQuaternion, aNextFrame.myTransforms[aBoneIndex].myQuaternion, delta);
		CU::Vec3f S = CU::Vec3f::Lerp(aCurrentFrame.myTransforms[aBoneIndex].myScale, aNextFrame.myTransforms[aBoneIndex].myScale, delta);

		//scale
		boneLocalSpaceTransform(1, 1) = S.x;
		boneLocalSpaceTransform(2, 2) = S.y;
		boneLocalSpaceTransform(3, 3) = S.z;

		//rot
		boneLocalSpaceTransform = R.GetRotationMatrix4X4() * boneLocalSpaceTransform;

		//translation
		boneLocalSpaceTransform(4, 1) = T.x;
		boneLocalSpaceTransform(4, 2) = T.y;
		boneLocalSpaceTransform(4, 3) = T.z;

	}
	CU::Matrix4x4f boneToModelSpaceTransform = boneLocalSpaceTransform * aParentsTransform;
	Bone& bone = aModel->myBones[aBoneIndex];

	aFinalTransformList[aBoneIndex] = bone.myBindToBone * boneToModelSpaceTransform;
	aLocalSpaceTransformList[aBoneIndex] = boneLocalSpaceTransform;
	aModelSpaceTransformList[aBoneIndex] = boneToModelSpaceTransform;
	for (size_t i = 0; i < bone.myChildrenCount; i++)
	{
		UpdateAnimations(aCurrentFrame, aNextFrame, boneToModelSpaceTransform, aFinalTransformList, aModelSpaceTransformList, aLocalSpaceTransformList, aModel, bone.myChildren[i], delta);
	}
}

void ISTE::AnimationDrawerSystem::Draw()
{

	BindShader();
	size_t size = myRenderCommands.size(); 
	for (int i = 0; i < size; i++)
	{
		AnimationCommand& com = myRenderCommands[i];

		//binds obj buffer
		BindBuffer(com);

		myCtx->myRenderStateManager->SetSampleState(com.myAdressMode, com.mySamplerState);

		for (int i = 0; i < com.myMeshCount; i++)
		{
			//binds Textures
			myCtx->myTextureManager->PsBindTexture(com.myTextureIDs[i][ALBEDO_MAP], 0);
			myCtx->myTextureManager->PsBindTexture(com.myTextureIDs[i][NORMAL_MAP], 1);
			myCtx->myTextureManager->PsBindTexture(com.myTextureIDs[i][MATERIAL_MAP], 2);

			//renders model
			myCtx->myModelManager->BindMesh(com.myModelId, i);
			myCtx->myModelManager->RenderMesh(com.myModelId, i);
		}
	}
}

void ISTE::AnimationDrawerSystem::DrawDepth(std::set<EntityID>& anEntityList)
{
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& modelPool = scene.GetComponentPool<ModelComponent>();
	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
	ComponentPool& materialPool = scene.GetComponentPool<MaterialComponent>();
	ComponentPool& animatorPool = scene.GetComponentPool<AnimatorComponent>();


	ComponentMask checkMask;
	checkMask.set(myCtx->mySceneHandler->GetId<ModelComponent>());
	checkMask.set(myCtx->mySceneHandler->GetId<TransformComponent>());
	checkMask.set(myCtx->mySceneHandler->GetId<MaterialComponent>());
	checkMask.set(myCtx->mySceneHandler->GetId<AnimatorComponent>());

	auto& models = myCtx->myModelManager->GetModelList();
	auto& animations = myCtx->myAnimationManager->GetAnimationList();


	myCtx->myDX11->GetContext()->VSSetShader(myVertexShader.Get(), 0, 0);
	myCtx->myDX11->GetContext()->IASetInputLayout(myInputLayout.Get());
	myCtx->myDX11->GetContext()->PSSetShader(nullptr, 0, 0);

	for (auto& entity : anEntityList)
	{
		if ((checkMask & scene.GetEntity(entity).myMask) != checkMask)
			continue;

		AnimatorComponent* animatorComp = (AnimatorComponent*)animatorPool.Get(GetEntityIndex(entity));
		AnimationCommand command;
		{
			MaterialComponent* materialComp = (MaterialComponent*)materialPool.Get(GetEntityIndex(entity));
			memcpy(command.myTextureIDs, materialComp->myTextures, sizeof(TextureID) * MAX_MATERIAL_COUNT * MAX_MESH_COUNT);
		}

		//processes model data
		{
			ModelComponent* modelData = (ModelComponent*)modelPool.Get(GetEntityIndex(entity));

			if (modelData->myModelId == ModelID(-1))
				continue;
			command.myModelId = modelData->myModelId;
			command.myColor = { modelData->myColor.x,modelData->myColor.y,modelData->myColor.z,1 };
			command.myUV = modelData->myUV;
			command.myUVScale = modelData->myUVScale;
			command.mySamplerState = modelData->mySamplerState;
			command.myAdressMode = modelData->myAdressMode;
			command.myEntityIndex = GetEntityIndex(entity);
			//oomfie... does a safety check in case it should be here at all
			//99.99% of the time its false so it should be fine... r-i-ight
			if (myCtx->myModelManager->GetModelType(modelData->myModelId) == ModelType::eTransparantModel)
			{
				auto* comp = scene.AssignComponent<CustomShaderComponent>(entity);
				comp->myShader = Shaders::eDefaultModelShader;
			}
		}

		//processses transform data
		{
			TransformComponent* transformData = (TransformComponent*)transformPool.Get(GetEntityIndex(entity));
			command.myTransform = transformData->myCachedTransform;
		}

		//updates animation
		{

			if (animatorComp->myCurrentAnimation == AnimationID(-1))
			{
				if (animatorComp->myAnimations[0] == AnimationID(-1))
					continue;
				animatorComp->myCurrentAnimation = animatorComp->myAnimations[0];
			}

			Animation* currentAnimation = animations.GetByIterator(animatorComp->myCurrentAnimation);

			float durationInFrames = animatorComp->myTimer * currentAnimation->myFps;
			int currentFrame = (int)std::floor(durationInFrames);
			int nextFrame = currentFrame + 1;
			float delta = durationInFrames - (float)currentFrame;

			Model* model = models.GetByIterator(command.myModelId);
			command.myMeshCount = model->myMeshCount;
			memcpy(command.myBindPoseInverse, animatorComp->myPose.myBindSpaceTransforms, sizeof(CU::Matrix4x4f)* MAX_BONE_COUNT);
		 }


		myCtx->myRenderStateManager->SetSampleState(command.myAdressMode, command.mySamplerState);
		//binds obj buffer
		BindBuffer(command);


		for (int i = 0; i < command.myMeshCount; i++)
		{
			BindBuffer(command);
			//renders model
			myCtx->myModelManager->BindMesh(command.myModelId, i);
			myCtx->myModelManager->RenderMesh(command.myModelId, i);
		}
	}
}
void ISTE::AnimationDrawerSystem::DrawDepth()
{
	myCtx->myDX11->GetContext()->VSSetShader(myVertexShader.Get(), 0, 0);
	myCtx->myDX11->GetContext()->IASetInputLayout(myInputLayout.Get());
	myCtx->myDX11->GetContext()->PSSetShader(nullptr, 0, 0);
	size_t size = myRenderCommands.size();
	for (int i = 0; i < size; i++)
	{
		AnimationCommand& com = myRenderCommands[i];

		//binds obj buffer
		BindBuffer(com);

		for (int i = 0; i < com.myMeshCount; i++)
		{
			//renders model
			myCtx->myModelManager->BindMesh(com.myModelId, i);
			myCtx->myModelManager->RenderMesh(com.myModelId, i);
		}
	}
}

void ISTE::AnimationDrawerSystem::DrawVisualization()
{
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& modelPool = scene.GetComponentPool<ModelComponent>();
	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>(); 
	ComponentPool& animatorPool = scene.GetComponentPool<AnimatorComponent>();
	auto& models = myCtx->myModelManager->GetModelList(); 

	TransformComponent* tC = nullptr;
	AnimatorComponent* aC = nullptr;
	ModelComponent* mC = nullptr;
	Model* model = nullptr;
	EntityIndex eIdx;
	ModelID mID;

	for (EntityID entity : myEntities[0])
	{
		eIdx = GetEntityIndex(entity);
		tC = (TransformComponent*)transformPool.Get(eIdx);
		aC = (AnimatorComponent*)animatorPool.Get(eIdx);
		mC = (ModelComponent*)modelPool.Get(eIdx);
		
		model = models.GetByIterator(mC->myModelId);
		CreateSkeletonVisualisation(tC->myCachedTransform, tC->myCachedTransform, aC, model, 0);
	}
	for (EntityID entity : myEntities[1])
	{
		eIdx = GetEntityIndex(entity);
		tC = (TransformComponent*)transformPool.Get(eIdx);
		aC = (AnimatorComponent*)animatorPool.Get(eIdx);
		mC = (ModelComponent*)modelPool.Get(eIdx);

		model = models.GetByIterator(mC->myModelId);
		CreateSkeletonVisualisation(tC->myCachedTransform, tC->myCachedTransform, aC, model, 0);
	}

}

void ISTE::AnimationDrawerSystem::ClearCommands()
{ 
	myRenderCommands.clear();
} 

void ISTE::AnimationDrawerSystem::CreateSkeletonVisualisation(const CU::Matrix4x4f& aModelTransform, const CU::Matrix4x4f& aBoneF, ISTE::AnimatorComponent* aAnimationThing, ISTE::Model* aModel, int aBoneIndex)
{
	ISTE::Bone& bone = aModel->myBones[aBoneIndex];
	CU::Matrix4x4f& boneT = aAnimationThing->myPose.myModelSpaceBoneTransforms[aBoneIndex] * aModelTransform;

	for (size_t i = 0; i < bone.myChildrenCount; i++)
	{
		myCtx->myGraphicsEngine->GetDebugDrawer().AddDynamicLineCommand(
			LineCommand{
				aBoneF.GetTranslationV3(),
				boneT.GetTranslationV3(),
				{0,1,0}
			}
		);

		CreateSkeletonVisualisation(aModelTransform, boneT, aAnimationThing, aModel, bone.myChildren[i]);
	}
}

float ISTE::AnimationDrawerSystem::PerformeInterpolationUGLY(AnimationBlendData& data, InterpolationID id)
{
	switch (id.myType)
	{
	case InterpolationType::eConstant:
	{
		data.myInterpConstant[id.myDataIndex].myTimer += Context::Get()->myTimeHandler->GetDeltaTime();

		return data.myInterpConstant[id.myDataIndex].myTValue;
	}

	case InterpolationType::eLinear:
	{
		data.myInterpLinear[id.myDataIndex].myTimer += Context::Get()->myTimeHandler->GetDeltaTime() * data.myInterpLinear[id.myDataIndex].mySpeed;

		if (data.myInterpLinear[id.myDataIndex].myTimer >= 1)
			data.myInterpLinear[id.myDataIndex].myTimer = 0;

		return data.myInterpLinear[id.myDataIndex].myTimer;
	}

	case InterpolationType::eSin:
	{
		data.myInterpSin[id.myDataIndex].myTimer += Context::Get()->myTimeHandler->GetDeltaTime() * data.myInterpSin[id.myDataIndex].mySpeed;  
		return ((sin(data.myInterpSin[id.myDataIndex].myTimer) * 0.5) + 0.5) * data.myInterpSin[id.myDataIndex].myModifier;
	}

	}
	return 1.f;
}



bool ISTE::AnimationDrawerSystem::InitBuffers()
{
	HRESULT result;
	
	{
	D3D11_BUFFER_DESC objBufferDesc = {};
	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objBufferDesc.ByteWidth = sizeof(ObjectBuffer);
	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myObjBuffer.GetAddressOf());
	if (FAILED(result))
		return false;
	}

	{
	D3D11_BUFFER_DESC objBufferDesc = {};
	objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	objBufferDesc.ByteWidth = sizeof(CommonGBuffer);
	objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myCommonGBuffer.GetAddressOf());
	if (FAILED(result))
		return false;
	}
	return true;
}

bool ISTE::AnimationDrawerSystem::InitShaders(std::string aVSPath, std::string aPSPath)
{
	ID3D11Device* aDevice = myCtx->myDX11->GetDevice();
	HRESULT result;
	std::string vsData;
	DX11::CreateVertexShader(aVSPath, myVertexShader.GetAddressOf(), vsData);
	DX11::CreatePixelShader(aPSPath, myPixelShader.GetAddressOf()); 
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"UV",
				0,
				DXGI_FORMAT_R32G32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"NORMAL",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"BINORMAL",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"TANGENT",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"BONES",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
			{
				"WEIGHTS",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0
			},
		};

		int elementCount = sizeof(layout) / sizeof(layout[0]);
		result = aDevice->CreateInputLayout(layout, elementCount, vsData.data(), vsData.size(), myInputLayout.GetAddressOf());
		if (FAILED(result))
			return false;
	}
	return true;
}

void ISTE::AnimationDrawerSystem::BindBuffer(AnimationCommand& aCommand)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	HRESULT res;
	{
	res = context->Map(myObjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	ObjectBuffer* OBdata = (ObjectBuffer*)(resource.pData);
	OBdata->myTransform = aCommand.myTransform;
	OBdata->myPrevTransform = aCommand.myPrevTransform;
	memcpy(OBdata->myBoneTransform, aCommand.myBindPoseInverse, sizeof(CU::Matrix4x4f) * MAX_BONE_COUNT);
	context->Unmap(myObjBuffer.Get(), 0);
	}

	{
	res = context->Map(myCommonGBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	CommonGBuffer* CommonGData = (CommonGBuffer*)(resource.pData);
	CommonGData->myColor.x		= aCommand.myColor.x;
	CommonGData->myColor.y		= aCommand.myColor.y;
	CommonGData->myColor.z		= aCommand.myColor.z;
	CommonGData->myIndex		= aCommand.myEntityIndex; 
	CommonGData->myRenderFlags  = (float)aCommand.myRenderFlags; 
	context->Unmap(myCommonGBuffer.Get(), 0);
	}

	context->VSSetConstantBuffers(2, 1, myObjBuffer.GetAddressOf());
	context->PSSetConstantBuffers(2, 1, myCommonGBuffer.GetAddressOf()); 
}

void ISTE::AnimationDrawerSystem::BindShader()
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	context->VSSetShader(myVertexShader.Get(), 0, 0);
	context->PSSetShader(myPixelShader.Get(), 0, 0);
	context->IASetInputLayout(myInputLayout.Get());
}

