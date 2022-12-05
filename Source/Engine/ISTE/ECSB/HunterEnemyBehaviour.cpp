#include "HunterEnemyBehaviour.h"
#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/ComponentsAndSystems/EnemyStatsComponent.h"
#include "ISTE/ComponentsAndSystems/AttackValueComponent.h"
#include "ISTE/Navigation/PathFinding.h"
#include "ISTE/Physics/ComponentsAndSystems/TriggerComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Time/TimerDefines.h"
#include "ISTE/Events/EventHandler.h"
#include "ISTE/VFX/VFXHandler.h"

#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/MaterialComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/DecalComponent.h"
#include "ISTE/Graphics/Resources/TextureManager.h"

#include "ISTE/Audio/AudioSource.h"

#include <fstream>
#include "Json/json.hpp"

#include "ISTE/CU/UtilityFunctions.hpp"
#include "ISTE/CU/MemTrack.hpp"

// Other Enemy Behaviours
#include "IdleEnemyBehaviour.h"

#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"

// Player Behaviour
#include "PlayerBehaviour.h"

#include "ISTE/Physics/ComponentsAndSystems/SphereTriggerComponent.h"

#include "ISTE/Graphics/ComponentAndSystem/CullingSystem.h"
#include "ISTE/CU/Geometry/Intersection.h"


void HunterLookAt(ISTE::TransformComponent& aTransform, CU::Vec3f& aLookAtPos, float aLerpValue = 1)
{
	//lookat
	CU::Vec3f tmp = ISTE::MovementMath::GetUnitVector3(aLookAtPos, aTransform.myPosition);
	tmp = CU::Vec3f::Lerp(aTransform.myCachedTransform.GetForwardV3(), tmp, aLerpValue);
	tmp.y = 0;
	tmp.Normalize();
	CU::Vec3f up = { 0,1,0 };
	CU::Vec3f right = up.Cross(tmp);
	right.Normalize();

	CU::Matrix3x3f rotMatrix;
	rotMatrix.GetRow(1) = { right.x,right.y, right.z, 0 };
	rotMatrix.GetRow(2) = { up.x,up.y, up.z, 0 };
	rotMatrix.GetRow(3) = { tmp.x,tmp.y, tmp.z, 0 };

	CU::Quaternionf quat(rotMatrix);

	aTransform.myQuaternion = quat;
	aTransform.myEuler.ToEulerAngles();
	//lookat end
}

bool CanShootPlayer(const CU::Vec3f& aPos, const CU::Vec3f& aOtherPos)
{
	Cell* grid = ISTE::Context::Get()->mySystemManager->GetSystem<ISTE::CullingSystem>()->GetGrid();

	std::set<EntityID> entities;

	CU::Line3D line;
	CU::Box3D box;

	line.myFrom = aPos;
	line.myTo = aOtherPos;

	for (size_t i = 0; i < UwUSplits * UwUSplits; i++)
	{
		box.myMin = grid[i].myPosition - (grid[i].mySize / 2.f);
		box.myMax = grid[i].myPosition + (grid[i].mySize / 2.f);

		//line.myFrom.y = 0;
		//line.myTo.y = 0;

		if (CU::LineBoxIntersection(line, box))
		{
			for (auto& entity : grid[i].myEntities)
			{
				entities.insert(entity);
			}
		}
	}

	ISTE::Scene& scene = ISTE::Context::Get()->mySceneHandler->GetActiveScene();

	ISTE::ComponentPool& transformPool = scene.GetComponentPool<ISTE::TransformComponent>();
	ISTE::ComponentPool& modelPool = scene.GetComponentPool<ISTE::ModelComponent>();

	ISTE::TransformComponent* entityTransform;
	ISTE::ModelComponent* entityModel;



	for (auto& entity : entities)
	{
		if (!scene.IsEntityIDValid(entity) || (scene.GetEntity(entity).myMask & ISTE::Context::Get()->mySceneHandler->GetFBM()) != 0
			|| scene.GetComponent<ISTE::AttackValueComponent>(entity) != nullptr || scene.GetLayer(entity) == "IgnoreVisibility")
			continue;

		entityTransform = (ISTE::TransformComponent*)transformPool.Get(GetEntityIndex(entity));
		entityModel = (ISTE::ModelComponent*)modelPool.Get(GetEntityIndex(entity));

		ISTE::Model* model = ISTE::Context::Get()->myModelManager->GetModel(entityModel->myModelId);

		CU::Matrix4x4f inverseMatrix = CU::Matrix4x4f::Inverse(entityTransform->myCachedTransform);

		CU::Vec4f temp = CU::Vec4f(aPos.x, aPos.y, aPos.z, 1) * inverseMatrix;
		line.myFrom = CU::Vec3f(temp.x, temp.y, temp.z);

		temp = CU::Vec4f(aOtherPos.x, aOtherPos.y, aOtherPos.z, 1) * inverseMatrix;
		line.myTo = CU::Vec3f(temp.x, temp.y, temp.z);

		CU::Vec3f recalcedMin;
		CU::Vec3f recalcedMax;

		CU::Vec3f size = model->myAABB.myMax - model->myAABB.myMin;
		CU::Vec3f midPoint = model->myAABB.myMin + (size / 2.f);

		//size = size * entityTransform->myScale;

		recalcedMin = midPoint - (size / 2.f);
		recalcedMax = midPoint + (size / 2.f);

		if (size.LengthSqr() <= 5)
			continue;

		//if (CU::IsInside(line.myFrom, recalcedMin, recalcedMax) || CU::IsInside(line.myTo, recalcedMin, recalcedMax))
		//	continue;

		box.myMin = recalcedMin;
		box.myMax = recalcedMax;


		if (CU::LineBoxIntersection(line, box))
			return false;



	}

	return true;
}

ISTE::HunterEnemyBehaviour::~HunterEnemyBehaviour()
{
	Context::Get()->myTimeHandler->RemoveTimer("HunterTimer_" + std::to_string(myHostId));
}

void ISTE::HunterEnemyBehaviour::Init()
{
	myActiveScene = &ISTE::Context::Get()->mySceneHandler->GetActiveScene();
	myStats = myActiveScene->GetComponent<EnemyStatsComponent>(myHostId);
	myTransform = myActiveScene->GetComponent<ISTE::TransformComponent>(myHostId);

	UpdateWithDatabase(Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourDatabase());

	myIsActive = false;
	myHasAnOrder = false;
	myHasIdleBehaviour = true;
	myCanAttack = true;
	myWarningPlayer = false;

	myTimer = 0.f;

	myProjectileCount = 0;

	myPlayerLastSeenPos = myTransform->myPosition; // Default;

	if (myActiveScene->GetComponent<IdleEnemyBehaviour>(myHostId) == nullptr)
	{
		myIsActive = true;
		myHasIdleBehaviour = false;
	}

	StepTimer stepTimer;
	stepTimer.callback = [this]() {
		if (CanSeePlayer())
		{
			Reset();
			BuildPath();
		} };
	stepTimer.delay = TicksPerSecond(20);
	stepTimer.name = "HunterTimer_" + std::to_string(myHostId);
	Context::Get()->myTimeHandler->AddTimer(stepTimer);

	myAnimationHelper.SetEntityID(myHostId);
	myAnimationHelper.MapAnimation(HunterAnimations::eIdle, myIdleAnim, AH_LOOPING, 0, 0);
	myAnimationHelper.MapAnimation(HunterAnimations::eDead, myDeadAnim, 0, 0, 0);
	myAnimationHelper.MapAnimation(HunterAnimations::eAttack, myAttackAnim, 0, 0, 0);
	myAnimationHelper.MapAnimation(HunterAnimations::eMovement, myMovmentAnim, AH_LOOPING, 0, 0);
	myAnimationHelper.MapAnimation(HunterAnimations::ePreExplosion, myPreExplodeAnim, AH_LOOPING, 0, 0);
	myAnimationHelper.MapAnimation(HunterAnimations::eExplode, myExplodeAnim, 0, 0, 0, 15.f);
	myAnimationHelper.ForcePlay(HunterAnimations::eIdle);

	myAudioSource = myActiveScene->GetComponent<AudioSource>(myHostId);

}

void ISTE::HunterEnemyBehaviour::Update(float aDeltaTime)
{

	if (myFrameWait)
	{
		if (!myWaitForExplosion)
		{
			myActiveScene->DestroyEntity(myExplosionId);
			myActiveScene->DestroyEntity(myCircleVFX);
			myStats->myIsDead = true;
			myIsActive = false;
			myWaitForExplosion = true;
		}

		//myAnimationHelper.Update();

		if(!myAnimationHelper.IsPlaying())
			myActiveScene->DestroyEntity(myHostId);

		return;
	}

	if (myWaitForAnim && !myStats->myIsDead)
	{
		if (!myAnimationHelper.IsPlaying())
		{
			myAnimationHelper.Play(HunterAnimations::eIdle);
			myWaitForAnim = false;
		}
	}


	if (myStats->myIsDead)
	{
		Explosion();
		Context::Get()->myEventHandler->InvokeEvent(EventType::BorisDied, 0);
		if (myIsElite) Context::Get()->myEventHandler->InvokeEvent(EventType::BorisEliteDied, 0);
		Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::MinionTookDamage, myStats->myMaxHealth);
		Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::PlayerGainExperience, myExperience);
		//myAnimationHelper.Play(HunterAnimations::eDead);
		/*	myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myCurrentAnimation = myDeadAnim;
			myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myLoopingFlag = false;*/
		//if (!myAnimationHelper.IsPlaying())
		//{
		//	Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::PlayerGainExperience, myExperience);
		//	myActiveScene->DestroyEntity(myExplosionId);
		//	myActiveScene->DestroyEntity(myCircleVFX);
		//	myActiveScene->DestroyEntity(myHostId);
		//}
		return;
	}

	if (!myIsActive)
	{
		myWaitForReAllignment = false;
		if (!CanSeePlayer())
		{
			if (!myActiveScene->GetComponent<ISTE::IdleEnemyBehaviour>(myHostId)->Reseting())
			{
				myAnimationHelper.Play(HunterAnimations::eMovement);
			}
			else
			{
				myAnimationHelper.Play(HunterAnimations::eIdle);
			}
			return;
		}
		else
		{
			if (myHasIdleBehaviour)
			{
				myActiveScene->GetComponent<ISTE::IdleEnemyBehaviour>(myHostId)->Reset();
				myActiveScene->GetComponent<ISTE::IdleEnemyBehaviour>(myHostId)->SetIsActive(false);
				myIsActive = true;
			}
			return;
		}
	}

	if (myWarningPlayer)
	{
		myWarningDelay -= aDeltaTime;

		if (myWarningDelay <= 0)
			Explosion();

		return;
	}

	if (CanSeePlayer())
	{
		HunterLookAt(*myTransform, myPlayerLastSeenPos, 20 * aDeltaTime);
	}

	//if (CanSeePlayer())
	//{
	//	Reset();

	//	BuildPath();

	//	HuntMode(aDeltaTime);

	//	return;
	//}
	if (!myIsStaled)
	{
		HuntMode(aDeltaTime);
	}
	else
	{
		myStalTimer += aDeltaTime;
		if (myStalTimer >= myStalTime)
			myIsStaled = false;

		myAnimationHelper.Play(HunterAnimations::eIdle);
	}

	if (myHasIdleBehaviour && !myHasAnOrder && !myWarningPlayer)
	{
		myTimer += 1.f * aDeltaTime;

		if (myTimer >= myMaxTimer)
		{
			myTimer = 0.f;
			myIsActive = false;
			myActiveScene->GetComponent<ISTE::IdleEnemyBehaviour>(myHostId)->SetIsActive(true);
			//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myCurrentAnimation = myIdleAnim;
			//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myAnimationState = AnimationState::ePlaying;
			//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myLoopingFlag = true;
			myAnimationHelper.Play(HunterAnimations::eIdle);
		}
	}
}

void ISTE::HunterEnemyBehaviour::OnTrigger(EntityID aId)
{
	if (myStats->myIsDead || !myIsActive)
		return;

	if (Context::Get()->mySceneHandler->GetActiveScene().GetComponent<IdleEnemyBehaviour>(aId))
	{

		EnemyStatsComponent* eComp = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<EnemyStatsComponent>(aId);

		if (eComp == nullptr || eComp->myIsDead)
			return;

		TransformComponent* otherTransform = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(aId);
		TransformComponent* transform = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myHostId);

		CU::Vec3f diff = transform->myPosition - otherTransform->myPosition;
		CU::Vec3f toDir = transform->myCachedTransform.GetForwardV3();

		if (myHasAnOrder)
		{
			toDir = transform->myPosition - myOrders.back();
		}

		if (toDir.Dot(diff) < 0.0)
			return;

		myWaitForReAllignment = true;
		myHasAnOrder = true;
		myOrders = std::queue<CU::Vec3f>();

		CU::Vec3f toPoint;

		if (transform->myCachedTransform.GetRightV3().Dot(diff) > 0)
			toPoint = transform->myPosition + (transform->myCachedTransform.GetRightV3().GetNormalized()) / 2.f;
		else
			toPoint = transform->myPosition - (transform->myCachedTransform.GetRightV3().GetNormalized()) / 2.f;


		const int destinationInNavmesh = NavMesh::FindTriangleIndexFromPosInside(toPoint, Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh());


		if (destinationInNavmesh < 0)
			toPoint = NavMesh::GetRealisticPointInNavmeshFromPoint(Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh(), transform->myPosition, toPoint);
		else
			toPoint.y = NavMesh::GetAproximatedY(Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh().triangles[destinationInNavmesh], toPoint);

		myOrders.push(toPoint);
	}
}

void ISTE::HunterEnemyBehaviour::Reset()
{
	if (!myHasAnOrder || myWaitForReAllignment) // Do not reset if already empty.
		return;

	myHasAnOrder = false;
	std::queue<CU::Vec3f> empty;
	std::swap(myOrders, empty);

	myTimer = 0.f;
}

bool ISTE::HunterEnemyBehaviour::CanSeePlayer()
{
	ISTE::TransformComponent* playersTransform = myActiveScene->GetComponent<ISTE::TransformComponent>(myActiveScene->GetPlayerId());

	if (playersTransform == nullptr || myWaitForReAllignment)
		return false;

	const CU::Vec2f enemyToPlayer = { playersTransform->myPosition.x - myTransform->myPosition.x, playersTransform->myPosition.z - myTransform->myPosition.z };

	const bool playerIsToFarAway = (enemyToPlayer.Length() > myFindRadius) || abs(myTransform->myPosition.y - playersTransform->myPosition.y) >= 3;

	if (playerIsToFarAway)// || !CanShootPlayer(myTransform->myPosition + CU::Vec3f(0, 0.9f, 0), playersTransform->myPosition + CU::Vec3f(0, 0.9f, 0)))
		return false;

	if (enemyToPlayer.Length() <= mySuicideDistance && !myStats->myIsDead)
	{
		myWarningPlayer = true;
		PreExplosion();
		return false;
	}

	myPlayerLastSeenPos = playersTransform->myPosition;
	myTimer = 0.f;

	return true;
}

void ISTE::HunterEnemyBehaviour::HuntMode(float aDeltaTime)
{

	if (myWarningPlayer)
	{
		return;
	}

	if (!myHasAnOrder)
	{
		if (!myWaitForAnim)
		{
			myAnimationHelper.Play(HunterAnimations::eIdle);
		}
		return;
	}

	if (!myWaitForAnim)
	{
		myAnimationHelper.Play(HunterAnimations::eMovement);
	}

	//myAnimationHelper.Play(HunterAnimations::eMovement);

	CU::Vec3f point = myOrders.front();
	float length;
	CU::Vec3f unitVector = MovementMath::GetUnitVector3(myTransform->myPosition, point, length);

	HunterLookAt(*myTransform, point, 20 * aDeltaTime);

	ISTE::TransformComponent* playersTransform = myActiveScene->GetComponent<ISTE::TransformComponent>(myActiveScene->GetPlayerId());

	if (length < 0.1f)
	{
		if (myWaitForReAllignment)
			myWaitForReAllignment = false;

		myLastToPoint = point;
		myOrders.pop();
		if (myOrders.empty())
		{
			myHasAnOrder = false;
			if (CanSeePlayer() && length <= myStats->myMiniumDistance && abs(playersTransform->myPosition.y - myTransform->myPosition.y) <= 3.f)
				Attack(aDeltaTime);

			if (myAnimationHelper.GetCurrentMap() != HunterAnimations::eAttack)
				myAnimationHelper.Play(HunterAnimations::eIdle);
		}
	}
	else
	{
		myTransform->myPosition = myTransform->myPosition + unitVector * aDeltaTime * mySpeed;
		if (CanSeePlayer() && length <= myStats->myMiniumDistance && abs(playersTransform->myPosition.y - myTransform->myPosition.y) <= 3.f)
			Attack(aDeltaTime);

		if (myAnimationHelper.GetCurrentMap() != HunterAnimations::eAttack)
			myAnimationHelper.Play(HunterAnimations::eMovement);
	}
}

void ISTE::HunterEnemyBehaviour::BuildPath()
{
	if (myHasAnOrder)
		return;

	// Set offset
	const CU::Vec3f unitVector = MovementMath::GetUnitVector3(myTransform->myPosition, myPlayerLastSeenPos);

	// Set start/destination
	const CU::Vec3f& start = myTransform->myPosition;
	CU::Vec3f destination = { myPlayerLastSeenPos.x - unitVector.x * myKeepDistance, myPlayerLastSeenPos.y - unitVector.y * myKeepDistance, myPlayerLastSeenPos.z - unitVector.z * myKeepDistance };

	NavMesh::NavMesh& nav = Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh();

	const int destinationExistInNav = NavMesh::FindTriangleIndexFromPosInside(destination, nav);

	// Find
	if (destinationExistInNav < 0)
	{
		destination = NavMesh::GetRealisticPointInNavmeshFromPoint(nav, start, destination);
	}
	else
	{
		NavMesh::NavMeshTriangle& tri = nav.triangles[destinationExistInNav];
		float app = NavMesh::GetAproximatedY(tri, destination);
		destination.y = app;
	}

	NavMesh::FindPointPath(start, destination, myOrders);
	//NavMesh::FindPointPath(start, NavMesh::GetRealisticPointInNavmeshFromPoint(Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh(), start, destination), myOrders);

	if (myOrders.empty())
		return;

	CU::Vec3f fPoint = myOrders.front();

	if (fPoint == myLastToPoint && myOrders.size() > 1)
		myOrders.pop();

	myHasAnOrder = true;
}

void ISTE::HunterEnemyBehaviour::Attack(float aDeltaTime)
{
	ISTE::TransformComponent* playersTransform = myActiveScene->GetComponent<ISTE::TransformComponent>(myActiveScene->GetPlayerId());

	HunterLookAt(*myTransform, playersTransform->myPosition, 1);

	if (!myCanAttack)
	{
		if (!myWaitForAnim)
		{
			myAnimationHelper.Play(HunterAnimations::eIdle);
		}
		return;
	}



	if (!CanShootPlayer(myTransform->myPosition + CU::Vec3f(0, 0.9f, 0), playersTransform->myPosition + CU::Vec3f(0, 0.9f, 0)))
		return;

	
	myIsStaled = true;
	myStalTimer = 0;
	TransformComponent* transform = myActiveScene->GetComponent<TransformComponent>(myHostId);
	myAnimationHelper.Play(HunterAnimations::eAttack);
	myWaitForAnim = true;
	myAudioSource->Play((int)HunterSounds::eAttack);
	myProjectileCount += 1;

	EntityID entityID = myActiveScene->NewEntity();

	std::string id = "projectile" + std::to_string(entityID);
	// Create a new entity
	myProjectileIds[id] = entityID;

	// Assign all components
	myActiveScene->AssignComponent<SphereTriggerComponent>(myProjectileIds[id]);
	TransformComponent* attackTransfrom = myActiveScene->AssignComponent<TransformComponent>(myProjectileIds[id]);
	AttackValueComponent* attack = myActiveScene->AssignComponent<AttackValueComponent>(myProjectileIds[id]);

	// Set all values
	attack->myAttackValue = myStats->myDamage;
	attack->myTag = "EnemyAttack";
	attack->myExtraInfo = "Projectile";
	attack->myIdentifier = std::to_string(myHostId) + "_" + id;

	attackTransfrom->myPosition = { transform->myPosition.x, transform->myPosition.y + 0.9f, transform->myPosition.z };
	attackTransfrom->myScale = { myStats->myAttackSize, myStats->myAttackSize, myStats->myAttackSize };

	//CU::Vec2f unitVector = MovementMath::GetUnitVector(myTransform->myPosition, myPlayerLastSeenPos);

	CU::Vec3f unitVector = playersTransform->myPosition - myTransform->myPosition;
	//unitVector.y = 0;
	unitVector.Normalize();

	// Attack Speed Timer
	myCanAttack = false;
	CountDown attackSpeedTimer;
	attackSpeedTimer.name = "HunterAttackSpeedTimer_" + attack->myIdentifier;
	attackSpeedTimer.duration = myStats->myAttackSpeed;
	attackSpeedTimer.callback = [this]() { myCanAttack = true; };
	Context::Get()->myTimeHandler->AddTimer(attackSpeedTimer);

	//// Stay Trigger Timer
	//CountDown attackStayTimer;
	//attackSpeedTimer.name = "HunterAttackStayTimer_" + std::to_string(myHostId);

	// spawn vfx
	int vfxId = Context::Get()->myVFXHandler->SpawnVFX("Boris_Projectile", attackTransfrom->myPosition);
	Context::Get()->myVFXHandler->SetVFXRotation(vfxId, attackTransfrom->myEuler.GetAngles());

	attackSpeedTimer.name = "HunterAttack_" + attack->myIdentifier;
	attackSpeedTimer.duration = myStats->myStayRate;
	attackSpeedTimer.callback = [this, idC = myProjectileIds[id], vfxId, attackTransfrom]() {
		Context::Get()->mySceneHandler->GetActiveScene().DestroyEntity(idC);
		Context::Get()->myVFXHandler->SpawnVFX("Boris_Projectile_Impact", attackTransfrom->myPosition);
		Context::Get()->myVFXHandler->DeactivateVFX(vfxId);
	};
	Context::Get()->myTimeHandler->AddTimer(attackSpeedTimer);

	// Movement On Projectile For Boris
	UpdateTimer attackMovementTimer;
	attackMovementTimer.name = "HunterProjectileTimer_" + attack->myIdentifier;
	attackMovementTimer.duration = myStats->myStayRate - 0.01f;
	attackMovementTimer.callback = [speed = myStats->mySpeed, unitVector, attackTransfrom, vfxId]() {
		float delta = Context::Get()->myTimeHandler->GetDeltaTime();
		attackTransfrom->myPosition = { attackTransfrom->myPosition.x + unitVector.x * delta * speed,
										attackTransfrom->myPosition.y + unitVector.y * delta * speed,
										attackTransfrom->myPosition.z + unitVector.z * delta * speed };
		Context::Get()->myVFXHandler->SetVFXPosition(vfxId, attackTransfrom->myPosition);
	};
	Context::Get()->myTimeHandler->AddTimer(attackMovementTimer);

}

void ISTE::HunterEnemyBehaviour::PreExplosion()
{
	if (myCircleVFX != INVALID_ENTITY)
		return;

	//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myCurrentAnimation = myExplodeAnim;
	//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myAnimationState = AnimationState::ePlaying;
	//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myLoopingFlag = false;
	//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myTimer = 0;

	myAnimationHelper.ForcePlay(HunterAnimations::ePreExplosion);
	myWaitForAnim = true;
	myAudioSource->Play((int)HunterSounds::ePreExplosion, ASP_EXLUSIVE | ASP_IGNOREIFACTIVE);

	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

	myCircleVFX = scene.NewEntity();
	scene.SetEntityName(myCircleVFX, "FuckOff");
	TransformComponent* transform = scene.AssignComponent<TransformComponent>(myCircleVFX);
	transform->myPosition = myTransform->myPosition;
	transform->myScale = { myExplosionRange, myExplosionRange, myExplosionRange };
	transform->myEuler.SetRotation(0, 0, 0);
	
	// spawn vfx
	Context::Get()->myVFXHandler->SpawnVFX("Boris_Warning", transform->myPosition);
}

void ISTE::HunterEnemyBehaviour::Explosion()
{
	// Create a new entity
	TransformComponent* transform = myActiveScene->GetComponent<TransformComponent>(myHostId);
	myExplosionId = myActiveScene->NewEntity();

	// Assign all components
	SphereTriggerComponent* trigger = myActiveScene->AssignComponent<SphereTriggerComponent>(myExplosionId);
	TransformComponent* attackTransfrom = myActiveScene->AssignComponent<TransformComponent>(myExplosionId);
	AttackValueComponent* attack = myActiveScene->AssignComponent<AttackValueComponent>(myExplosionId);

	// Set all values
	attack->myAttackValue = myExplosionDamage;
	attack->myTag = "EnemyAttack";

	attackTransfrom->myPosition = { transform->myPosition.x, transform->myPosition.y, transform->myPosition.z };

	trigger->myRadius = myExplosionRange;

	myActiveScene->GetComponent<ModelComponent>(myHostId)->myModelId = myExplodeModel;

	myAnimationHelper.ForcePlay(HunterAnimations::eExplode);
	myAudioSource->Play((int)HunterSounds::eExplosion, ASP_EXLUSIVE | ASP_IGNOREIFACTIVE);

	EntityID decalId = myActiveScene->NewEntity();
	TransformComponent* decalTransform = myActiveScene->AssignComponent<TransformComponent>(decalId);
	decalTransform->myPosition = transform->myPosition;
	decalTransform->myEuler.SetRotation({ 0, CU::GetRandomFloat(0.f, 360.f), 0});
	decalTransform->myScale *= 5.f;
	DecalComponent* decalDecal = myActiveScene->AssignComponent<DecalComponent>(decalId);
	MaterialComponent* decalMaterial = myActiveScene->AssignComponent<MaterialComponent>(decalId);
	decalMaterial->myTextures[0][ALBEDO_MAP] = Context::Get()->myTextureManager->LoadTexture(L"../Assets/Sprites/Decals/Boris_Splatt.dds").myValue;
	decalDecal->myLerpValues = { 0, 1, 1 };
	//UpdateTimer attackMovementTimer;
	//attackMovementTimer.name = "explosiontimer";
	//attackMovementTimer.duration = 0.00f;
	//attackMovementTimer.callback = [scene = myActiveScene,explosion = explosionID]() {
	//	scene->DestroyEntity(explosion);
	//};
	//Context::Get()->myTimeHandler->AddTimer(attackMovementTimer);
	//myActiveScene->RemoveComponent<ModelComponent>(myHostId);
	myActiveScene->RemoveComponent<TriggerComponent>(myHostId);
	myActiveScene->DestroyEntity(myCircleVFX);
	myFrameWait = true;
	Context::Get()->myTimeHandler->RemoveTimer("HunterAttackSpeedTimer_" + std::to_string(myHostId));

	{
		EntityID tmpEntity = Context::Get()->mySceneHandler->GetActiveScene().NewEntity();
		TransformComponent* tra = Context::Get()->mySceneHandler->GetActiveScene().AssignComponent<TransformComponent>(tmpEntity);
		tra->myPosition = transform->myPosition;
		tra->myScale = { 1,1,1 };

		CountDown textStayTimer;
		textStayTimer.name = "tmpEntity";
		textStayTimer.duration = 2.01f;
		textStayTimer.callback = [this, tmpEntity]() {
			Context::Get()->mySceneHandler->GetActiveScene().DestroyEntity(tmpEntity);
		};
		Context::Get()->myTimeHandler->AddTimer(textStayTimer);
	}

	Context::Get()->myVFXHandler->SpawnVFX("Boris_Explosion", transform->myPosition);
}
void ISTE::HunterEnemyBehaviour::UpdateWithDatabase(CU::Database<true>& aBehaviourDatabase)
{
	mySpeed = aBehaviourDatabase.Get<float>("HunterSpeed");
	myFindRadius = aBehaviourDatabase.Get<float>("HunterFindRadius");
	myMaxTimer = aBehaviourDatabase.Get<float>("HunterWatchTime");
	myKeepDistance = aBehaviourDatabase.Get<float>("HunterKeepDistance");
	myStats->myMaxHealth = myStats->myHealth = aBehaviourDatabase.Get<float>("HunterHealth");

	mySuicideDistance = aBehaviourDatabase.Get<float>("HunterExplosionDistance");
	myWarningDelay = aBehaviourDatabase.Get<float>("HunterWarningDelay");
	myExplosionRange = aBehaviourDatabase.Get<float>("HunterExplosionRange");
	myExplosionDamage = aBehaviourDatabase.Get<float>("HunterExplosionDamage");

	myStats->myDamage = aBehaviourDatabase.Get<float>("HunterWeaponDamage");
	myStats->myAttackSize = aBehaviourDatabase.Get<float>("HunterWeaponSize");
	myStats->myAttackSpeed = aBehaviourDatabase.Get<float>("HunterWeaponAttackSpeed");
	myStats->myStayRate = aBehaviourDatabase.Get<float>("HunterWeaponStay");
	myStats->mySpeed = aBehaviourDatabase.Get<float>("HunterWeaponSpeed");
	myStats->myMiniumDistance = aBehaviourDatabase.Get<float>("HunterWeaponMinDistance");

	myStats->myName = aBehaviourDatabase.Get<std::string>("HunterName");

	myExperience = aBehaviourDatabase.Get<int>("HunterExperience");
	if (myIsElite)
	{
		CU::Database<true>& behaviour = Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourDatabase();
		myStats->myDamage = behaviour.Get<float>("EliteHunterDamage");
		myStats->myHealth = behaviour.Get<float>("EliteHunterHealth");
		myStats->myMaxHealth = myStats->myHealth;
	}
}