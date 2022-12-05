#include "IdleEnemyBehaviour.h"
#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/CU/Database.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/ComponentsAndSystems/EnemyStatsComponent.h"
#include "ISTE/ComponentsAndSystems/AttackValueComponent.h"
#include "ISTE/Navigation/PathFinding.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Time/TimerDefines.h"
#include "ISTE/Graphics/ComponentAndSystem/CustomShaderComponent.h"

#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"

#include "ISTE/Audio/AudioSource.h"
#include "ISTE/VFX/VFXHandler.h"

#include <fstream>
#include "Json/json.hpp"

#include "ISTE/CU/MemTrack.hpp"

void ISTE::IdleEnemyBehaviour::Init()
{
	myActiveScene = &ISTE::Context::Get()->mySceneHandler->GetActiveScene();
	myStats = myActiveScene->GetComponent<EnemyStatsComponent>(myHostId);
	CU::Database<true>& behaviours = myActiveScene->GetBehaviourDatabase();
	myRandomEngine = std::mt19937(std::random_device()());


	myRadius = behaviours.Get<float>("IdleRadius");
	mySpeed = behaviours.Get<float>("IdleSpeed");
	myMaxTimer = behaviours.Get<float>("IdleRest");
	myDamageShaderDuration = behaviours.Get<float>("IdleDamageDuration");
	myStats->myHealth = behaviours.Get<float>("IdleHealth");
	myStats->myIsDead = false;

	myRandomFloat = std::uniform_real_distribution(-myRadius, myRadius);
	myTimer = 0.f;

	myTransform = myActiveScene->GetComponent<ISTE::TransformComponent>(myHostId);
	myOrgin = myActiveScene->GetComponent<ISTE::TransformComponent>(myHostId)->myPosition;

	myIsActive = true;
	myHasAnOrder = false;
	myOnRest = false;
	myOnIFrameMode = false;
}

void ISTE::IdleEnemyBehaviour::Update(float aDeltaTime)
{
	if (!myIsActive)
		return;

	if (myStats->myIsDead)
		return;

	MoveToIdle(aDeltaTime); // Move towards a idle position if the myOrders har orders.

	BuildPath(); // Create a idle order queue if it's empty.

	if (myOnRest) // Some rest between new idle positions.
	{
		myTimer += 1.f * aDeltaTime;
		if (myTimer >= myMaxTimer)
		{
			myOnRest = false;
			myTimer = 0.f;
		}
	}
}

void ISTE::IdleEnemyBehaviour::Reset()
{
	myHasAnOrder = false;
	myOnRest = false;
	myTimer = 0.f;
	std::queue<CU::Vec3f> empty;
	std::swap(myOrders, empty);
}

void ISTE::IdleEnemyBehaviour::OnTrigger(EntityID aId)
{

	if (myStats->myIsDead)
		return;

	AttackValueComponent* attacked = myActiveScene->GetComponent<AttackValueComponent>(aId);
	if (attacked == nullptr) // Haha, not attacked this time.
		return;

	if (attacked != nullptr && attacked->myExtraInfo == "SecondaryAttack")
	{
		Context::Get()->myTimeHandler->InvokeTimer("PlayerProjectileAttackStayTimer" + attacked->myIdentifier);

		Context::Get()->myTimeHandler->RemoveTimer("PlayerProjectileAttackStayTimer" + attacked->myIdentifier);
		Context::Get()->myTimeHandler->RemoveTimer("PlayerProjectileTimer" + attacked->myIdentifier);

		//if we wanna do single hits in a very ugly way we could
		attacked->myExtraInfo = "";

	}

	if (attacked->myTag != "PlayerRangedAttack" && attacked->myTag != "PlayerMeleeAttack")
		return;

	if (attacked->myExtraInfo == "DamageOverTime")
		myStats->myHealth -= attacked->myAttackValue * Context::Get()->myTimeHandler->GetDeltaTime();
	else
		myStats->myHealth -= attacked->myAttackValue;

	//DamageShader();

	if (myStats->myHealth <= 0.f)
		myStats->myIsDead = true;


	if (myOnIFrameMode)
		return;

	if (attacked->myExtraInfo == "PrimaryAttack")
	{
		Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
		AudioSource* source = scene.GetComponent<AudioSource>(myHostId);
		if (source != nullptr)
			source->Play((int)SharedSounds::eTookDamage);

		CU::Vec3f vfxPos = scene.GetComponent<TransformComponent>(myHostId)->myPosition;
		vfxPos.y += 1.f;
		Context::Get()->myVFXHandler->SpawnVFX("Player_Primary_Impact", vfxPos);
	}

	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
	ModelComponent* model = scene.GetComponent<ModelComponent>(myHostId);
	model->myColor = CU::Vec3f(4.f, 1.f, 1.f);

	// I Frame Timer
	myOnIFrameMode = true;
	CountDown iFrameTimer;
	iFrameTimer.name = "EnemyIFrameTimer";
	iFrameTimer.duration = myDamageShaderDuration;
	iFrameTimer.callback = [this, model]() {
		myOnIFrameMode = false;
		model->myColor = CU::Vec3f(1.f, 1.f, 1.f);
	};
	Context::Get()->myTimeHandler->AddTimer(iFrameTimer);
}

void ISTE::IdleEnemyBehaviour::MoveToIdle(float aDeltaTime)
{
	if (!myHasAnOrder)
		return;

	CU::Vec3f point = myOrders.front();
	float length;
	CU::Vec3f unitVector = MovementMath::GetUnitVector3(myTransform->myPosition, point, length);

	//lookat
	CU::Vec3f tmp = MovementMath::GetUnitVector3(point, myTransform->myPosition, length);
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

	myTransform->myQuaternion = quat;
	myTransform->myEuler.ToEulerAngles();
	//

	if (length < 0.1f)
	{
		myOrders.pop();

		if (myOrders.empty())
		{
			myHasAnOrder = false;
			myOnRest = true;
		}
	}
	else
		myTransform->myPosition = myTransform->myPosition + unitVector * mySpeed * aDeltaTime;
}

void ISTE::IdleEnemyBehaviour::BuildPath()
{
	if (myHasAnOrder || myOnRest)
		return;

	// Set
	const CU::Vec3f& start = myTransform->myPosition;
	CU::Vec3f destination = { myOrgin.x + myRandomFloat(myRandomEngine), myOrgin.y, myOrgin.z + myRandomFloat(myRandomEngine) };

	NavMesh::NavMesh& nav = Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh();

	int destinationExistInNav = NavMesh::FindTriangleIndexFromPosInside(destination, nav);

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


	if (myOrders.empty())
		return;

	myHasAnOrder = true;
}

void ISTE::IdleEnemyBehaviour::DamageShader()
{
	if (Context::Get()->mySceneHandler->GetActiveScene().GetComponent<AnimatorComponent>(myHostId) == nullptr)
		return;

	CustomShaderComponent* shader = myActiveScene->AssignComponent<CustomShaderComponent>(myHostId);
	shader->myShader = Shaders::eEnemyDamaged;
	shader->myElaspedTime = 0.f;
	shader->myDuration = myDamageShaderDuration - 0.01f;

	// Shader Timer
	CountDown shaderTimer;
	shaderTimer.name = "EnemyShaderTimer";
	shaderTimer.duration = myDamageShaderDuration;
	shaderTimer.callback = [this]() { myActiveScene->RemoveComponent<CustomShaderComponent>(myHostId); };
	Context::Get()->myTimeHandler->AddTimer(shaderTimer);
}