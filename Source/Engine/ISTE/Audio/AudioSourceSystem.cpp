#include "AudioSourceSystem.h"

#include "AudioSource.h"
#include "ISTE/Scene/Scene.h"

#include "ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h"

#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"

#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/Graphics/ComponentAndSystem/BillboardRenderCommand.h"

void ISTE::AudioSourceSystem::Init()
{
	myAudioSourceID = Context::Get()->myTextureManager->LoadTexture(L"../EngineAssets/Images/AudioIcons/AudioSourceIcon.dds", true);
}

void ISTE::AudioSourceSystem::StopAll()
{
	ComponentPool& audioSourcePool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<AudioSource>();

	for (auto& entityID : myEntities[0])
	{
		((AudioSource*)audioSourcePool.Get(GetEntityIndex(entityID)))->StopAll();
	}
}

void ISTE::AudioSourceSystem::PauseAll(bool aShouldPause)
{
	ComponentPool& audioSourcePool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<AudioSource>();

	for (auto& entityID : myEntities[0])
	{
		((AudioSource*)audioSourcePool.Get(GetEntityIndex(entityID)))->PauseAll(aShouldPause);
	}
}

void ISTE::AudioSourceSystem::StopAll(SoundTypes aType)
{
	ComponentPool& audioSourcePool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<AudioSource>();

	for (auto& entityID : myEntities[0])
	{

		AudioSource* aS = (AudioSource*)audioSourcePool.Get(GetEntityIndex(entityID));

		if (aS->mySoundType == aType)
		{
			aS->StopAll();
		}
	}
}

void ISTE::AudioSourceSystem::PauseAll(SoundTypes aType, bool aShouldPause)
{
	ComponentPool& audioSourcePool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<AudioSource>();

	for (auto& entityID : myEntities[0])
	{
		AudioSource* aS = (AudioSource*)audioSourcePool.Get(GetEntityIndex(entityID));

		if (aS->mySoundType == aType)
		{
			aS->PauseAll(aShouldPause);
		}
	}
}

void ISTE::AudioSourceSystem::PrepareAudioSourceIcons()
{
	SpriteDrawerSystem* sSystem = Context::Get()->mySystemManager->GetSystem<SpriteDrawerSystem>();

	BillboardCommand bCom;
	bCom.myColor = { 1,1,1,1 };
	bCom.myUVScale = { 1,1 };
	bCom.myUVStart = { 0,0 };
	bCom.myUVEnd = { 1,1 };
	bCom.myTextures = myAudioSourceID;

	ComponentPool& tPool = Context::Get()->mySceneHandler->GetActiveScene().GetComponentPool<TransformComponent>();

	ISTE::TransformComponent* tForm = NULL;

	for (auto& entityID : myEntities[0])
	{
		bCom.myTransform = ((TransformComponent*)tPool.Get(GetEntityIndex(entityID)))->myCachedTransform;
		bCom.myEntityIndex = GetEntityIndex(entityID);

		sSystem->AddBillboardCommand(bCom);

	}
}
