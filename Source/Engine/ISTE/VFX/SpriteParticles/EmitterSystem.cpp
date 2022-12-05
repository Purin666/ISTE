#include "EmitterSystem.h"

#include "ISTE/Context.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"

#include "ISTE/VFX/SpriteParticles/Sprite3DParticleHandler.h"

#include "ISTE/VFX/SpriteParticles/EmitterComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"

// for billboards
#include "ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h"
#include "ISTE/Graphics/ComponentAndSystem/BillboardRenderCommand.h"
#include "ISTE/Graphics/Resources/TextureManager.h"

void ISTE::EmitterSystem::Init()
{
	myCtx = Context::Get();
	myEmitterIcon = myCtx->myTextureManager->LoadTexture(L"../EngineAssets/Images/VFXIcons/Emitter3DIcon.dds", true);
}

void ISTE::EmitterSystem::SpawnEmitters()
{
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& ePool = scene.GetComponentPool<EmitterComponent>();
	ComponentPool& tPool = scene.GetComponentPool<TransformComponent>();

	EmitterComponent*	e = nullptr;
	TransformComponent* t = nullptr;

	for (EntityID id : myEntities[0])
	{
		const EntityIndex index = GetEntityIndex(id);

		e = (EmitterComponent*)ePool.Get(index);
		t = (TransformComponent*)tPool.Get(index);

		e->myEmitterId = myCtx->mySprite3DParticleHandler->SpawnEmitter(e->myEmitterType, t->myPosition);
	}
}

void ISTE::EmitterSystem::UpdateEmitterPositions()
{
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& ePool = scene.GetComponentPool<EmitterComponent>();
	ComponentPool& tPool = scene.GetComponentPool<TransformComponent>();

	EmitterComponent* e = nullptr;
	TransformComponent* t = nullptr;
	//TransformComponent* p = nullptr;

	for (EntityID id : myEntities[0])
	{
		const EntityIndex index = GetEntityIndex(id);
		
		e = (EmitterComponent*)ePool.Get(index);
		t = (TransformComponent*)tPool.Get(index);
		CU::Vec3f pos = t->myCachedTransform.GetTranslationV3();

		//const EntityID parentId = scene.GetParent(id);
		//if (parentId != INVALID_ENTITY)
		//{
		//	const EntityIndex parentIndex = GetEntityIndex(parentId);
		//	p = (TransformComponent*)tPool.Get(parentIndex);
		//
		//	if (p != nullptr)
		//		pos += p->myPosition;
		//}

		myCtx->mySprite3DParticleHandler->SetEmitterPosition(e->myEmitterId, pos);
	}
}

void ISTE::EmitterSystem::PrepareEmitterIcons()
{
	SpriteDrawerSystem* billDrawer = myCtx->mySystemManager->GetSystem<SpriteDrawerSystem>();
	
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& tPool = scene.GetComponentPool<TransformComponent>();

	TransformComponent* t = nullptr;

	BillboardCommand billCom;
	billCom.myTransform.GetRow(1) *= 0.1f;
	billCom.myTransform.GetRow(2) *= 0.1f;
	billCom.myTextures = myEmitterIcon;
	
	for (EntityID id : myEntities[0])
	{
		const EntityIndex index = GetEntityIndex(id);
		t = (TransformComponent*)tPool.Get(index);

		billCom.myTransform = t->myCachedTransform;
		billCom.myEntityIndex = index;
		billDrawer->AddBillboardCommand(billCom);
	}
}
