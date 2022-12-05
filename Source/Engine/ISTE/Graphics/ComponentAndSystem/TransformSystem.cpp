#include "TransformSystem.h"
#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"

#include "ISTE/ECSB/ECSDefines.hpp"

#include "ISTE/Events/EventHandler.h"

ISTE::TransformSystem::TransformSystem()
{
	Context::Get()->myEventHandler->RegisterCallback(ISTE::EventType::EntityParentedToSpace, "TransformSystem", [this](EntityID aId) {EntityParentedCallback(aId); });
	Context::Get()->myEventHandler->RegisterCallback(ISTE::EventType::EntityUnParented, "TransformSystem", [this](EntityID aId) {EntityUnParentedCallback(aId); });
}

void ISTE::TransformSystem::Init()
{
	myCtx = Context::Get(); 

	//there should be a on init func call to resize this to its appropriate size
	//this is the same for all mentions of VVV 
	myLastFrameTransforms.resize(MAX_ENTITIES);
}

void ISTE::TransformSystem::EntityParentedCallback(EntityID aChild)
{
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

	TransformComponent* pComp = scene.GetComponent<TransformComponent>(scene.GetParent(aChild));
	TransformComponent* cComp = scene.GetComponent<TransformComponent>(aChild);

	if (pComp == nullptr || cComp == nullptr)
		return;

	UpdateToSpace(pComp, cComp, aChild);
}

void ISTE::TransformSystem::EntityUnParentedCallback(EntityID aChild)
{
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

	TransformComponent* pComp = scene.GetComponent<TransformComponent>(scene.GetParent(aChild));
	TransformComponent* cComp = scene.GetComponent<TransformComponent>(aChild);

	if (pComp == nullptr || cComp == nullptr)
		return;

	UpdateFromSpace(pComp, cComp, aChild);
}


void ISTE::TransformSystem::UpdateToSpace(TransformComponent* aParent, TransformComponent* aChild, EntityID aChildId)
{
	CU::Matrix4x4f temp = aChild->myCachedTransform * CU::Matrix4x4f::Inverse(aParent->myCachedTransform);
	aChild->myPosition = temp.GetTranslationV3();
	aChild->myScale = temp.DecomposeScale();
	CU::Matrix3x3f rotation(temp);
	rotation.GetRow(1) = rotation.GetRow(1) / aChild->myScale.x;
	rotation.GetRow(2) = rotation.GetRow(2) / aChild->myScale.y;
	rotation.GetRow(3) = rotation.GetRow(3) / aChild->myScale.z;
	CU::Quaternionf tor = CU::Quaternionf(rotation);
	aChild->myQuaternion = tor;
	aChild->myEuler.ToEulerAngles();

	myLastFrameTransforms[GetEntityIndex(aChildId)] = *aChild;
}

void ISTE::TransformSystem::UpdateFromSpace(TransformComponent* aParent, TransformComponent* aChild, EntityID aChildId)
{
	CU::Matrix4x4f temp = aChild->myCachedTransform * aParent->myCachedTransform;
	aChild->myPosition = temp.GetTranslationV3();
	aChild->myScale = temp.DecomposeScale();
	CU::Matrix3x3f rotation(temp);
	rotation.GetRow(1) = rotation.GetRow(1) / aChild->myScale.x;
	rotation.GetRow(2) = rotation.GetRow(2) / aChild->myScale.y;
	rotation.GetRow(3) = rotation.GetRow(3) / aChild->myScale.z;
	CU::Quaternionf tor = CU::Quaternionf(rotation);
	aChild->myQuaternion = tor;
	aChild->myEuler.ToEulerAngles();

	myLastFrameTransforms[GetEntityIndex(aChildId)] = *aChild;
}

void ISTE::TransformSystem::UpdateTransform()
{ 
	Scene* scene = &myCtx->mySceneHandler->GetActiveScene();
	ComponentPool* transformPool = &scene->GetComponentPool<TransformComponent>();
	
	//safety check
	if (transformPool == nullptr)
		return;

	//updates the transforms that have been changed  
	for (EntityID entity : myEntities[0])
	{
		CheckCache(entity, *scene, *transformPool);
	} 
}

void ISTE::TransformSystem::CheckTransform(TransformComponent* aTransform, EntityID aId, Scene& aScene, ComponentPool& aPool)
{

	TransformComponent* childTransform = (TransformComponent*)aPool.Get(GetEntityIndex(aId));

	CU::Matrix4x4f cScaleMatrix;
	CU::Matrix4x4f cRotMatrix;
	CU::Matrix4x4f cPosMatrix;

	cScaleMatrix(1, 1) = childTransform->myScale.x;
	cScaleMatrix(2, 2) = childTransform->myScale.y;
	cScaleMatrix(3, 3) = childTransform->myScale.z;
	cRotMatrix = childTransform->myQuaternion.GetRotationMatrix4X4();
	cPosMatrix(4, 1) = childTransform->myPosition.x;
	cPosMatrix(4, 2) = childTransform->myPosition.y;
	cPosMatrix(4, 3) = childTransform->myPosition.z;

	CU::Matrix4x4f cM = cScaleMatrix * cRotMatrix * cPosMatrix;

	childTransform->myCachedTransform = cM * aTransform->myCachedTransform;
	myLastFrameTransforms[GetEntityIndex(aId)] = *childTransform;

	Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::EntityMoved, aId);

	std::vector<EntityID> children = aScene.GetChildren(aId);

	for (auto& child : children)
	{
		CheckTransform(childTransform, child, aScene, aPool);
	}

}

void ISTE::TransformSystem::CheckCache(EntityID aId, Scene& aScene, ComponentPool& aPool)
{
	EntityIndex index = GetEntityIndex(aId);
	TransformComponent* transform = (TransformComponent*)aPool.Get(index);

	//since everything is a copy this could get a bit expensive
	const std::vector<EntityID>& children = aScene.GetChildren(aId);

	if (memcmp(transform, &myLastFrameTransforms[index], sizeof(TransformComponent)) != 0)
	{
		CachecTransform(transform, aId);

		for (auto& child : children)
		{
			CheckTransform(transform, child, aScene, aPool);
		}

		return;
	}

	for (auto& child : children)
	{
		CheckCache(child, aScene, aPool);
	}
}

void ISTE::TransformSystem::CachecTransform(TransformComponent* aTransform, EntityID aId)
{
	CU::Matrix4x4f ScaleMatrix;
	CU::Matrix4x4f RotMatrix;
	CU::Matrix4x4f PosMatrix;


	ScaleMatrix(1, 1) = aTransform->myScale.x;
	ScaleMatrix(2, 2) = aTransform->myScale.y;
	ScaleMatrix(3, 3) = aTransform->myScale.z;

	RotMatrix = aTransform->myQuaternion.GetRotationMatrix4X4();
	PosMatrix(4, 1) = aTransform->myPosition.x;
	PosMatrix(4, 2) = aTransform->myPosition.y;
	PosMatrix(4, 3) = aTransform->myPosition.z;

	aTransform->myCachedTransform = ScaleMatrix * RotMatrix * PosMatrix;

	EntityID parent = Context::Get()->mySceneHandler->GetActiveScene().GetParent(aId);

	if (parent != INVALID_ENTITY)
	{
		TransformComponent* cmp = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(parent);

		//shouldnt be needed
		if (cmp != nullptr)
			aTransform->myCachedTransform = aTransform->myCachedTransform * cmp->myCachedTransform;

	}

	Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::EntityMoved, aId);

	myLastFrameTransforms[GetEntityIndex(aId)] = *aTransform;
}
