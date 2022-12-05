#include "AnimatedShader.h"
#include <d3d11.h>


#include "ISTE/Graphics/Resources/Model.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Graphics/Resources/AnimationManager.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimationCommand.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/MaterialComponent.h"
#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/RenderStateManager.h" 

#include "ISTE/Time/TimeHandler.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"

#include "ISTE/Context.h"
#include "ISTE/Graphics/DX11.h"


struct ObjectBuffer {
	CU::Matrix4x4f myTransform;
	CU::Matrix4x4f myBoneTransform[MAX_BONE_COUNT];
};

bool ISTE::AnimatedShader::CreateBuffers()
{
	myCtx = Context::Get();
	HRESULT result;
	//FrameBuffer init
	{
		//move out in to a shader class later
		D3D11_BUFFER_DESC objBufferDesc = {};
		objBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		objBufferDesc.ByteWidth = sizeof(ObjectBuffer);
		objBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		objBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		result = myCtx->myDX11->GetDevice()->CreateBuffer(&objBufferDesc, NULL, myObjBuffer.GetAddressOf());
		if (FAILED(result))
			return false;
	}
	return true;

}

void ISTE::AnimatedShader::Render(EntityID aEntity)
{
	AnimationCommand rc;
	PrepEntity(aEntity, rc);
	
	//binds obj buffer
	BindObjBuffer(rc);



	for (int i = 0; i < rc.myMeshCount; i++)
	{

		//binds Textures
		myCtx->myTextureManager->PsBindTexture(rc.myTextureIDs[i][ALBEDO_MAP],   0);
		myCtx->myTextureManager->PsBindTexture(rc.myTextureIDs[i][NORMAL_MAP],   1);
		myCtx->myTextureManager->PsBindTexture(rc.myTextureIDs[i][MATERIAL_MAP], 2);

		//renders model
		myCtx->myModelManager->BindMesh(rc.myModelId, i);
		myCtx->myModelManager->RenderMesh(rc.myModelId, i);
	}
}

void ISTE::AnimatedShader::PrepEntity(EntityID aEntity, ISTE::AnimationCommand& aOutRenderCommand)
{ 

	{
	ModelComponent* modelData = myCtx->mySceneHandler->GetActiveScene().GetComponent<ModelComponent>(aEntity);
	
	aOutRenderCommand.myModelId = modelData->myModelId;
	aOutRenderCommand.myMeshCount = myCtx->myModelManager->GetMeshCount(modelData->myModelId);
	aOutRenderCommand.myColor = { modelData->myColor.x,modelData->myColor.y,modelData->myColor.z, 1 };
	aOutRenderCommand.myUV = modelData->myUV;
	aOutRenderCommand.myUVScale = modelData->myUVScale;
	myCtx->myRenderStateManager->SetSampleState(modelData->myAdressMode, modelData->mySamplerState);
	}

	{
	MaterialComponent* matComp = myCtx->mySceneHandler->GetActiveScene().GetComponent<MaterialComponent>(aEntity);
	memcpy(aOutRenderCommand.myTextureIDs, matComp->myTextures, sizeof(TextureID) * MAX_MATERIAL_COUNT * MAX_MESH_COUNT);
	}

	{
	TransformComponent* transformData = myCtx->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(aEntity);
	aOutRenderCommand.myTransform = transformData->myCachedTransform;
	}

	//updates animation
	{
	float myTimeDelta = myCtx->myTimeHandler->GetDeltaTime();

	AnimatorComponent* animatorData = myCtx->mySceneHandler->GetActiveScene().GetComponent<AnimatorComponent>(aEntity);
	if (animatorData->myCurrentAnimation == -1)
		return;
	
	Animation* animation = myCtx->myAnimationManager->GetAnimation(animatorData->myCurrentAnimation);
	 
	if (animatorData->myAnimationState == AnimationState::ePlaying)
		animatorData->myTimer += myTimeDelta;

	if (animatorData->myTimer > animation->myLengthInSeconds)
	{
		animatorData->myTimer = 0;

		if(!animatorData->myLoopingFlag)
			animatorData->myAnimationState = AnimationState::eEnded;
	}
	 
	float durationInFrames = animatorData->myTimer * animation->myFps;
	int currentFrame = (int)std::floor(durationInFrames);
	int nextFrame = currentFrame + 1;
	float delta = durationInFrames - (float)currentFrame;

	Model* model = myCtx->myModelManager->GetModel(aOutRenderCommand.myModelId);
	UpdateAnimations(animation->myKeyFrames[currentFrame], animation->myKeyFrames[nextFrame], CU::Matrix4x4f(), 
		aOutRenderCommand.myBindPoseInverse, animatorData->myPose.myModelSpaceBoneTransforms, animatorData->myPose.myLocalSpaceBoneTransforms, 
		model, 0, delta);
	}


	return;
}

void ISTE::AnimatedShader::BindObjBuffer(const AnimationCommand& aRc)
{
	ID3D11DeviceContext* context = myCtx->myDX11->GetContext();

	D3D11_MAPPED_SUBRESOURCE resource = { 0 };
	HRESULT res = context->Map(myObjBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	if (FAILED(res))
		return;
	ObjectBuffer* OBdata = (ObjectBuffer*)(resource.pData);

	OBdata->myTransform = aRc.myTransform;
	memcpy(OBdata->myBoneTransform, aRc.myBindPoseInverse, sizeof(CU::Matrix4x4f) * MAX_BONE_COUNT);

	context->Unmap(myObjBuffer.Get(), 0);
	context->VSSetConstantBuffers(2, 1, myObjBuffer.GetAddressOf());
	context->PSSetConstantBuffers(2, 1, myObjBuffer.GetAddressOf());
}


void ISTE::AnimatedShader::UpdateAnimations(Animation::KeyFrame& aCurrentFrame, Animation::KeyFrame& aNextFrame, CU::Matrix4x4f& aParentsTransform,
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
