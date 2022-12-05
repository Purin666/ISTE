#include "LurkerEnemyBehaviour.h"
#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/ComponentsAndSystems/EnemyStatsComponent.h"
#include "ISTE/ComponentsAndSystems/AttackValueComponent.h"
#include "ISTE/Navigation/PathFinding.h"
#include "ISTE/Physics/ComponentsAndSystems/TriggerComponent.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Time/TimerDefines.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"
#include "ISTE/Events/EventHandler.h"

#include "ISTE/Graphics/Resources/ModelManager.h"

#include <fstream>
#include "Json/json.hpp"

#include "ISTE/CU/UtilityFunctions.hpp"
#include "ISTE/CU/MemTrack.hpp"

#include "ISTE/Audio/AudioSource.h"


#include "ISTE/Physics/ComponentsAndSystems/SphereTriggerComponent.h"

// Other Enemy Behaviours
#include "IdleEnemyBehaviour.h"

// Player Behaviour
#include "PlayerBehaviour.h"

void LurkerLookAt(ISTE::TransformComponent& aTransform, CU::Vec3f& aLookAtPos, float aLerpValue = 1)
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

bool LurkerVectorAprox(CU::Vec3f vec1, CU::Vec3f vec2)
{
	if (vec1.x > vec2.x + 0.0001f || vec1.x < vec2.x - 0.0001f)
		return false;
	if (vec1.y > vec2.y + 0.0001f || vec1.y < vec2.y - 0.0001f)
		return false;
	if (vec1.z > vec2.z + 0.0001f || vec1.z < vec2.z - 0.0001f)
		return false;
}

ISTE::LurkerEnemyBehaviour::~LurkerEnemyBehaviour()
{
	Context::Get()->myTimeHandler->RemoveTimer("LurkerTimer" + std::to_string(myHostId));
}
 
void ISTE::LurkerEnemyBehaviour::Init()
{
	myActiveScene = &ISTE::Context::Get()->mySceneHandler->GetActiveScene();
	myStats = myActiveScene->GetComponent<EnemyStatsComponent>(myHostId);
	myTransform = myActiveScene->GetComponent<ISTE::TransformComponent>(myHostId);
	UpdateWithDatabase(Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourDatabase());

	myFirstAttackDelayCountDown.SetDuration(myFirstAttackTimerDuration);
	myFirstAttackDelayCountDown.SetOn(false);
	myFirstAttackDelayCountDown.SetCallback([this]()
		{
			myFirstAttackDelay = false;
			myTimerRunning = false;
		});
	//{ // Json Data Import

	//	std::ifstream stream("../Assets/Json/enemybehaviours.json");

	//	if (stream)
	//	{
	//		nlohmann::json reader;
	//		stream >> reader;
	//		mySpeed = reader["lurker"]["speed"].get<float>();
	//		myFindRadius = reader["lurker"]["findradius"].get<float>();
	//		myMaxTimer = reader["lurker"]["watchtime"].get<float>();
	//		myKeepDistance = reader["lurker"]["keepdistance"].get<float>();
	//		myStats->myHealth = reader["lurker"]["health"].get<float>();

	//		myStats->myDamage = reader["lurker"]["weapon"]["damage"].get<float>();
	//		myStats->myAttackSize = reader["lurker"]["weapon"]["size"].get<float>();
	//		myStats->myAttackSpeed = reader["lurker"]["weapon"]["attackspeed"].get<float>();
	//		myStats->myStayRate = reader["lurker"]["weapon"]["stayrate"].get<float>();

	//		myStats->myName = reader["lurker"]["name"].get<std::string>();
	//		myStats->myIsDead = false;

	//		//if (Context::Get()->mySceneHandler->GetActiveScene().GetComponent<AnimatorComponent>(myHostId) == nullptr)
	//		//{
	//		//	myIdleAnim = Context::Get()->myModelManager->LoadAnimation(myHostId, reader["lurker"]["animations"]["idle"].get<std::string>());
	//		//	myDeadAnim = Context::Get()->myModelManager->LoadAnimation(myHostId, reader["lurker"]["animations"]["dead"].get<std::string>());
	//		//	myAttackAnim = Context::Get()->myModelManager->LoadAnimation(myHostId, reader["lurker"]["animations"]["attack"].get<std::string>());
	//		//	myMovmentAnim = Context::Get()->myModelManager->LoadAnimation(myHostId, reader["lurker"]["animations"]["move"].get<std::string>());
	//		//}

	//		myExperience = reader["lurker"]["experience"].get<size_t>();


	//		if (myIsElite)
	//		{
	//			CU::Database<true>& behaviour = Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourDatabase();
	//			myStats->myDamage = behaviour.Get<float>("EliteLurkerDamage");
	//			myStats->myHealth = behaviour.Get<float>("EliteLurkerHealth");
	//			myStats->myMaxHealth = myStats->myHealth;
	//		}
	//	}
	//}

	myIsActive = false;
	myHasAnOrder = false;
	myHasIdleBehaviour = true;
	myCanAttack = true;

	myTimer = 0.f;

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
	stepTimer.name = "LurkerTimer" + std::to_string(myHostId);
	Context::Get()->myTimeHandler->AddTimer(stepTimer);


	myAnimationHelper.SetEntityID(myHostId);
	myAnimationHelper.MapAnimation(LurkerAnimations::eIdle, myIdleAnim, AH_LOOPING, 0, 0);
	myAnimationHelper.MapAnimation(LurkerAnimations::eDead, myDeadAnim, 0, 0, 0);
	myAnimationHelper.MapAnimation(LurkerAnimations::eAttack, myAttackAnim, 0, 0, 0);
	myAnimationHelper.MapAnimation(LurkerAnimations::eMovement, myMovmentAnim, AH_LOOPING, 0, 0);
	myAnimationHelper.ForcePlay(LurkerAnimations::eIdle);
	myAudioSource = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<AudioSource>(myHostId);
}

void ISTE::LurkerEnemyBehaviour::Update(float aDeltaTime)
{

	if (myWaitForAnim && !myStats->myIsDead)
	{
		//if (myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myAnimationState == AnimationState::eEnded)
		//{
		//	myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myCurrentAnimation = myIdleAnim;
		//	myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myAnimationState = AnimationState::ePlaying;
		//	myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myLoopingFlag = true;
		//	myWaitForAnim = false;
		//}

		if (!myAnimationHelper.IsPlaying())
		{
			myAnimationHelper.Play(LurkerAnimations::eIdle);
			myWaitForAnim = false;
		}
	}

	if (myStats->myIsDead)
	{

		//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myCurrentAnimation = myDeadAnim;
		//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myLoopingFlag = false;

		myAnimationHelper.Play(LurkerAnimations::eDead);
		myAudioSource->Play((int)LurkerSounds::eDeath, ASP_IGNOREIFACTIVE | ASP_EXLUSIVE);
		//if(myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myAnimationState == AnimationState::eEnded)
		//	Context::Get()->mySceneHandler->GetActiveScene().DestroyEntity(myHostId);
		
		if (!myHaveGivenExperience)
		{
			Context::Get()->myEventHandler->InvokeEvent(EventType::TimDied, 0);
			if (myStats->myIsElite) Context::Get()->myEventHandler->InvokeEvent(EventType::TimEliteDied, 0);
			Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::MinionTookDamage, myStats->myMaxHealth);
			Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::PlayerGainExperience, myExperience);
			myHaveGivenExperience = true;
		}
		if (!myAnimationHelper.IsPlaying())
		{
			Context::Get()->mySceneHandler->GetActiveScene().DestroyEntity(myHostId);
		}
		return;
	}

	myFirstAttackDelayCountDown.Update(aDeltaTime);

	if (!myIsActive)
	{
		myWaitForReAllignment = false;
		if (!CanSeePlayer())
		{
			if (!myActiveScene->GetComponent<ISTE::IdleEnemyBehaviour>(myHostId)->Reseting())
			{
				myAnimationHelper.Play(LurkerAnimations::eMovement);
			}
			else
			{
				myAnimationHelper.Play(LurkerAnimations::eIdle);
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

	if (CanSeePlayer())
	{
		LurkerLookAt(*myTransform, myPlayerLastSeenPos, 20 * aDeltaTime);
	}

	//if (CanSeePlayer())
	//{
	//	//Reset();
	//	//
	//	////BuildPath();
	//	//
	//	//LurkMode(aDeltaTime);

	//	return;
	//}

	LurkMode(aDeltaTime);

	if (myHasIdleBehaviour && !myHasAnOrder)
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
			myAnimationHelper.Play(LurkerAnimations::eIdle);
		}
	}
}

void ISTE::LurkerEnemyBehaviour::OnTrigger(EntityID aId)
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

void ISTE::LurkerEnemyBehaviour::Reset()
{
	if (!myHasAnOrder || myWaitForReAllignment) // Do not reset if already empty.
		return;

	myHasAnOrder = false;
	std::queue<CU::Vec3f> empty;
	std::swap(myOrders, empty);

	myTimer = 0.f;
}

bool ISTE::LurkerEnemyBehaviour::CanSeePlayer()
{
	ISTE::TransformComponent* playersTransform = myActiveScene->GetComponent<ISTE::TransformComponent>(myActiveScene->GetPlayerId());


	if (playersTransform == nullptr || myWaitForReAllignment)
		return false;


	const CU::Vec2f enemyToPlayer = { playersTransform->myPosition.x - myTransform->myPosition.x, playersTransform->myPosition.z - myTransform->myPosition.z };
	const bool playerIsToFarAway = (enemyToPlayer.Length() > myFindRadius) || abs(myTransform->myPosition.y - playersTransform->myPosition.y) >= 3;

	if (playerIsToFarAway)
		return false;

	myPlayerLastSeenPos = playersTransform->myPosition;
	myTimer = 0.f;

	return true;
}

void ISTE::LurkerEnemyBehaviour::LurkMode(float aDeltaTime)
{

	ISTE::TransformComponent* playersTransform = myActiveScene->GetComponent<ISTE::TransformComponent>(myActiveScene->GetPlayerId());
	if (!myHasAnOrder)
	{
		if (CanSeePlayer() && (myTransform->myPosition - playersTransform->myPosition).Length() <= (myKeepDistance + 0.5f) && abs(myTransform->myPosition.y - playersTransform->myPosition.y) <= 1)
			Attack();

		if (!myWaitForAnim)
		{
			//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myCurrentAnimation = myIdleAnim;
			//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myAnimationState = AnimationState::ePlaying;
			//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myLoopingFlag = true;
			myAnimationHelper.Play(LurkerAnimations::eIdle);
		}
		return;
	}

	if (!myWaitForAnim && !myTimerRunning)
	{
		//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myCurrentAnimation = myMovmentAnim;
		//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myAnimationState = AnimationState::ePlaying;
		//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myLoopingFlag = true;
		myAnimationHelper.Play(LurkerAnimations::eMovement);
	}

	float length;

	CU::Vec3f point = myOrders.front();

	CU::Vec3f unitVector = MovementMath::GetUnitVector3(myTransform->myPosition, point, length);

	LurkerLookAt(*myTransform, point, 20 * aDeltaTime);

	if (length < 0.1f)
	{
		if (myWaitForReAllignment)
			myWaitForReAllignment = false;

		myLastToPoint = point;
		myOrders.pop();
		if (myOrders.empty())
		{
			myHasAnOrder = false;
			if (CanSeePlayer() && (myTransform->myPosition - playersTransform->myPosition).Length() <= (myKeepDistance + 0.5f) && abs(myTransform->myPosition.y - playersTransform->myPosition.y) <= 1)
				Attack();
		}
	}
	else
	{
		if (length >= myKeepDistance)
		{
			myFirstAttackDelay = true;
			myTimerRunning = false;
			myFirstAttackDelayCountDown.SetOn(false);
			myFirstAttackDelayCountDown.SetDuration(myFirstAttackTimerDuration);
		}

		myTransform->myPosition = myTransform->myPosition + unitVector * aDeltaTime * mySpeed;
	}
}

void ISTE::LurkerEnemyBehaviour::BuildPath()
{
	if (myHasAnOrder)
		return;

	// Set offset
	const CU::Vec3f unitVector = MovementMath::GetUnitVector3(myTransform->myPosition, myPlayerLastSeenPos);

	// Set start/destination
	const CU::Vec3f& start = myTransform->myPosition;
	CU::Vec3f destination = { myPlayerLastSeenPos.x - unitVector.x * myKeepDistance, myPlayerLastSeenPos.y - unitVector.y, myPlayerLastSeenPos.z - unitVector.z * myKeepDistance };

	// Find
	NavMesh::NavMesh& nav = Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh();

	int destinationExistInNav = NavMesh::FindTriangleIndexFromPosInside(destination, nav);

	// Find
	if (destinationExistInNav < 0)
	{
		destination = myPlayerLastSeenPos;

		destinationExistInNav = NavMesh::FindTriangleIndexFromPosInside(destination, nav);

		if (destinationExistInNav < 0)
		{
			destination = NavMesh::GetRealisticPointInNavmeshFromPoint(nav, start, destination);
		}
	}
	else
	{
		NavMesh::NavMeshTriangle& tri = nav.triangles[destinationExistInNav];
		float app = NavMesh::GetAproximatedY(tri, destination);
		destination.y = app;
	}

	NavMesh::FindPointPath(start, destination, myOrders);
	
		if ((start - destination).Length() <= 0.2f)
		{
			myOrders = std::queue<CU::Vec3f>();
			return;
		}
	

		
	if (myOrders.empty())
		return;

	CU::Vec3f fPoint = myOrders.front();

	if ((fPoint - myLastToPoint).Length() < 0.01f && myOrders.size() > 1)
		myOrders.pop();

	if (myOrders.front() == start && myOrders.size() <= 1)
		return;

	if (myOrders.back() == start)
		return;


	myHasAnOrder = true;
}

void ISTE::LurkerEnemyBehaviour::Attack()
{


	//look at player
	ISTE::TransformComponent* playersTransform = myActiveScene->GetComponent<ISTE::TransformComponent>(myActiveScene->GetPlayerId());
	float length;

	LurkerLookAt(*myTransform, playersTransform->myPosition, 1);

	if (!myCanAttack)
	{
		if (!myWaitForAnim)
		{
			//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myCurrentAnimation = myIdleAnim;
			//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myAnimationState = AnimationState::ePlaying;
			//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myLoopingFlag = true;
			myAnimationHelper.Play(LurkerAnimations::eIdle);
		}
		return;
	}

	//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myCurrentAnimation = myAttackAnim;
	//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myAnimationState = AnimationState::ePlaying;
	//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myLoopingFlag = false;
	//myActiveScene->GetComponent<AnimatorComponent>(myHostId)->myTimer = 0;


	if (myFirstAttackDelay)
	{
		if (!myTimerRunning)
		{
			myTimerRunning = true;
			myFirstAttackDelayCountDown.SetOn(true);
		}

		return;
	}



	myAnimationHelper.Play(LurkerAnimations::eAttack);
	myWaitForAnim = true;

	//myAudioSource->Play((int)LurkerSounds::eAttack);

	// Create a new entity
	TransformComponent* transform = myActiveScene->GetComponent<TransformComponent>(myHostId);
	myAttackId = myActiveScene->NewEntity();

	// Assign all components
	SphereTriggerComponent* attackTrigger = myActiveScene->AssignComponent<SphereTriggerComponent>(myAttackId);
	attackTrigger->myOffset = { 0, 0.9f,0 };
	TransformComponent* attackTransfrom = myActiveScene->AssignComponent<TransformComponent>(myAttackId);
	attackTransfrom->myPosition = (transform->myPosition - transform->myCachedTransform.GetForwardV3() * myStats->myAttackSize);
	AttackValueComponent* attack = myActiveScene->AssignComponent<AttackValueComponent>(myAttackId);

	// Set all values
	attack->myAttackValue = myStats->myDamage;
	attack->myTag = "EnemyAttack";

	//attackTransfrom->myPosition = { transform->myPosition.x, transform->myPosition.y, transform->myPosition.z };
	attackTransfrom->myScale = { myStats->myAttackSize, myStats->myAttackSize, myStats->myAttackSize };

	// Attack Speed Timer
	myCanAttack = false;
	CountDown attackSpeedTimer;
	attackSpeedTimer.name = "LurkerAttackSpeedTimer" + std::to_string(myHostId);
	attackSpeedTimer.duration = myStats->myAttackSpeed;
	attackSpeedTimer.callback = [this]() { myCanAttack = true; };
	Context::Get()->myTimeHandler->AddTimer(attackSpeedTimer);

	// Stay Trigger Timer
	CountDown attackStayTimer;
	attackSpeedTimer.name = "LurkerAttackStayTimer" + std::to_string(myHostId);
	attackSpeedTimer.duration = 0.f; // myStats->myStayRate;
	attackSpeedTimer.callback = [this]() { Context::Get()->mySceneHandler->GetActiveScene().DestroyEntity(myAttackId); };
	Context::Get()->myTimeHandler->AddTimer(attackSpeedTimer);
}
void ISTE::LurkerEnemyBehaviour::UpdateWithDatabase(CU::Database<true>& aBehaviourDatabase)
{
	mySpeed = aBehaviourDatabase.Get<float>("LurkerSpeed");
	myFindRadius = aBehaviourDatabase.Get<float>("LurkerFindRadius");
	myMaxTimer = aBehaviourDatabase.Get<float>("LurkerWatchTime");
	myKeepDistance = aBehaviourDatabase.Get<float>("LurkerKeepDistance");
	myStats->myMaxHealth = myStats->myHealth = aBehaviourDatabase.Get<float>("LurkerHealth");

	myStats->myDamage = aBehaviourDatabase.Get<float>("LurkerWeaponDamage");
	myStats->myAttackSize = aBehaviourDatabase.Get<float>("LurkerWeaponSize");
	myStats->myAttackSpeed = aBehaviourDatabase.Get<float>("LurkerWeaponAttackSpeed");
	myStats->myStayRate = aBehaviourDatabase.Get<float>("LurkerWeaponStay");

	myStats->myName = aBehaviourDatabase.Get<std::string>("LurkerName");

	myExperience = aBehaviourDatabase.Get<int>("LurkerExperience");
	if (myIsElite)
	{
		CU::Database<true>& behaviour = Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourDatabase();
		myStats->myDamage = behaviour.Get<float>("EliteLurkerDamage");
		myStats->myHealth = behaviour.Get<float>("EliteLurkerHealth");
		myStats->myMaxHealth = myStats->myHealth;
	}
}