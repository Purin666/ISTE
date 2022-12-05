#include "EnemyBehaviourSystem.h"

#include "ISTE/Context.h"
#include "ISTE/WindowsWindow.h"

#include "ISTE/CU/InputHandler.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"

#include "ISTE/Graphics/Resources/ModelManager.h"

#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Physics/ComponentsAndSystems/TriggerComponent.h"
#include "ISTE/Physics/ComponentsAndSystems/SphereTriggerComponent.h"
#include "ISTE/ComponentsAndSystems/EnemyStatsComponent.h"
#include "ISTE/ECSB/IdleEnemyBehaviour.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h"
#include "ISTE/Graphics/Resources/TextureManager.h"

struct Box
{
	CU::Vec3f myMin;
	CU::Vec3f myMax;
};

bool RayAABB(const ISTE::ScreenHelper::Ray& aRay, const Box& aBox)
{
	CU::Vec3f tMin = (aBox.myMin - aRay.myOrigin) / aRay.myDir;
	CU::Vec3f tMax = (aBox.myMax - aRay.myOrigin) / aRay.myDir;

	float tmpX = 0;
	float tmpY = 0;
	float tmpZ = 0;

	tmpX = min(tMin.x, tMax.x);
	tmpY = min(tMin.y, tMax.y);
	tmpZ = min(tMin.z, tMax.z);

	CU::Vec3f t1 = CU::Vec3f(tmpX, tmpY, tmpZ);

	tmpX = max(tMin.x, tMax.x);
	tmpY = max(tMin.y, tMax.y);
	tmpZ = max(tMin.z, tMax.z);

	CU::Vec3f t2 = CU::Vec3f(tmpX, tmpY, tmpZ);

	float tNear = max(max(t1.x, t1.y), t1.z);
	float tFar = min(min(t2.x, t2.y), t2.z);

	return tNear <= tFar;
}


void ISTE::EnemyBehaviourSystem::UpdateHealthBarDrawCalls()
{
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

	SpriteDrawerSystem* sds				= Context::Get()->mySystemManager->GetSystem<SpriteDrawerSystem>();
	ComponentPool& transformPool		= scene.GetComponentPool<TransformComponent>();
	ComponentPool& eSCP					= scene.GetComponentPool<EnemyStatsComponent>();
	ComponentPool& behaviourPool		= scene.GetComponentPool<IdleEnemyBehaviour>();

	TransformComponent*		transform	= nullptr; 
	EnemyStatsComponent*	eSC			= nullptr;

	BillboardCommand bc;
	bc.myColor = { 1,0,0,1 }; 
	for (auto& entityID : myEntities[0])
	{
		EntityIndex index = GetEntityIndex(entityID); 
		BillboardCommand bc;
		bc.myColor = { 1,0,0,1 };
		bc.myTextures = myHeltBarTexture;
		transform = (TransformComponent*)transformPool.Get(index);
		eSC = (EnemyStatsComponent*)eSCP.Get(index); 
		if (eSC == nullptr)
			continue;
		bc.myTransform.GetTranslationV3() = transform->myCachedTransform.GetTranslationV3();
		bc.myTransform.GetTranslationV3().y += 2.f;
		if (eSC->myIsElite)
			bc.myTransform.GetTranslationV3().y += 0.5f;
		bc.myTransform.GetUp() *= 0.075f;
		bc.myTransform.GetRight() *= (max(eSC->myHealth,0) / eSC->myMaxHealth);
		if (eSC->myHealth < eSC->myMaxHealth)
			sds->AddBillboardCommand(bc);
	}
}
void ISTE::EnemyBehaviourSystem::KillAll()
{
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
	ComponentPool& eSCP = scene.GetComponentPool<EnemyStatsComponent>();
	EnemyStatsComponent* eSC = nullptr;

	for (auto& entityID : myEntities[0])
	{
		EntityIndex index = GetEntityIndex(entityID);
		eSC = (EnemyStatsComponent*)eSCP.Get(index);
		if (eSC == nullptr)
			continue;
		eSC->myHealth = -1.f;
		eSC->myIsDead = true;
	}
}


void ISTE::EnemyBehaviourSystem::Init()
{
	myHeltBarTexture = Context::Get()->myTextureManager->LoadTexture(L"../Assets/Sprites/Ui/Ui HUD/Ui_enemy_healthbar_small.dds");
}

EntityID ISTE::EnemyBehaviourSystem::GetEntityIDFromScreen()
{
	
	POINT point = myHelper.GetMouseRelativePosition();

	ScreenHelper::Ray ray = myHelper.GetRayFromPoint(point, (unsigned int*)&Context::Get()->myWindow->GetResolution());

	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();


	ComponentPool& transformPool		= scene.GetComponentPool<TransformComponent>();
	ComponentPool& triggerPool		= scene.GetComponentPool<TriggerComponent>();
	ComponentPool& sphereTriggerPool 	= scene.GetComponentPool<SphereTriggerComponent>();
	ComponentPool& modelPool		 	= scene.GetComponentPool<ModelComponent>();
	ComponentPool& eSCP				= scene.GetComponentPool<EnemyStatsComponent>();

	TransformComponent*		transform		= nullptr;
	TriggerComponent*		trigger		= nullptr;
	SphereTriggerComponent* sphereTrigger 	= nullptr;
	ModelComponent*			model			= nullptr;
	EnemyStatsComponent*	eSC				= nullptr;

	Box box;
	Model* m = nullptr;
	CU::Vec3f size;
	for (auto& entityID : myEntities[0])
	{
		EntityIndex index = GetEntityIndex(entityID);

		transform = (TransformComponent*)transformPool.Get(index);
		trigger = (TriggerComponent*)triggerPool.Get(index);
		sphereTrigger = (SphereTriggerComponent*)sphereTriggerPool.Get(index);
		model = (ModelComponent*)modelPool.Get(index);

		if (transform == nullptr || (trigger == nullptr && sphereTrigger == nullptr) || model == nullptr)
			continue;

		m = Context::Get()->myModelManager->GetModel(model->myModelId);

		size = m->myAABB.myMax - m->myAABB.myMin;

		if (trigger != nullptr)
		{
			box.myMin = (transform->myPosition + trigger->myOffset) - ((size * transform->myScale * trigger->mySize) / 2.f);
			box.myMax = (transform->myPosition + trigger->myOffset) + ((size * transform->myScale * trigger->mySize) / 2.f);
		}
		else if (sphereTrigger != nullptr)
		{
			box.myMin = (transform->myPosition + sphereTrigger->myOffset) - ((size * transform->myScale * sphereTrigger->myRadius) / 2.f);
			box.myMax = (transform->myPosition + sphereTrigger->myOffset) + ((size * transform->myScale * sphereTrigger->myRadius) / 2.f);
		}

		//box.myMin.y += 0.9f; //lazy offset
		//box.myMax.y += 0.9f;

		if (RayAABB(ray, box))
			return entityID;
	}


	return INVALID_ENTITY;
}
