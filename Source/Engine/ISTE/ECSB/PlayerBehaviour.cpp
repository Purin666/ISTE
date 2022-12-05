#include "PlayerBehaviour.h"

// ISTE
#include "ISTE/Context.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/PP_FS_Effects/ChromaticAberrationEffect.h"
#include "ISTE/Physics/ComponentsAndSystems/TriggerComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/ComponentsAndSystems/AttackValueComponent.h"
#include "ISTE/Graphics/DebugDrawer.h"
#include "ISTE/Navigation/PathFinding.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Time/TimerDefines.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"
#include "ISTE/Audio/AudioHandler.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Graphics/Resources/AnimationManager.h"
#include "ISTE/Graphics/ComponentAndSystem/MaterialComponent.h"
#include "ISTE/Events/EventHandler.h"
#include "ISTE/StateStack/StateManager.h"
#include "ISTE/StateStack/StateType.h"

#include "ISTE/ComponentsAndSystems/EnemyBehaviourSystem.h"
#include "ISTE/ECSB/ProjectileBlockBehaviour.h"

#include "ISTE/Time/LocalStepTimer.h"

// VFX
#include "ISTE/VFX/VFXHandler.h"
#include "ISTE/VFX/ModelVFX/ModelVFXHandler.h"
#include "ISTE/VFX/SpriteParticles/Sprite2DParticleHandler.h"

#include "ISTE/Audio/AudioSource.h"

// CU
#include "ISTE/CU/InputHandler.h"
#include "ISTE/CU/UtilityFunctions.hpp"
#include "ISTE/CU/MemTrack.hpp"
#include "ISTE/Text/TextComponent.h"
#include "ISTE/Graphics/Resources/TextureManager.h"

#include "ISTE/Graphics/ComponentAndSystem/AnimationDrawerSystem.h"

// stdlib & other
#include "../CU/Database.h"

// temp for the sphere ability // mathias
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/CustomShaderComponent.h"


#include "ISTE/ComponentsAndSystems/EnemyStatsComponent.h"
#include "ISTE/ECSB/BossBobBehaviour.h"

#include "ISTE/Physics/ComponentsAndSystems/SphereTriggerComponent.h"

//calculates if there is a clear line of sight //should be in some type of commonutilities
#include "ISTE/Graphics/ComponentAndSystem/CullingSystem.h"
#include "ISTE/CU/Geometry/Intersection.h"

#include "ISTE/Graphics/DebugDrawer.h"

void DrawBox(const CU::Box3D& aBox)
{
	ISTE::LineCommand line;
	line.myColor = { 0,10,0 };

	ISTE::DebugDrawer& drawer = ISTE::Context::Get()->myGraphicsEngine->GetDebugDrawer();

	line.myFromPosition = aBox.myMin;
	line.myToPosition = aBox.myMin;
	line.myToPosition.z = aBox.myMax.z;
	drawer.AddStaticLineCommand(line);
	line.myToPosition = aBox.myMin;
	line.myToPosition.x = aBox.myMax.x;
	drawer.AddStaticLineCommand(line);
	line.myToPosition = aBox.myMin;
	line.myToPosition.y = aBox.myMax.y;
	drawer.AddStaticLineCommand(line);

	line.myFromPosition = aBox.myMax;
	line.myToPosition = aBox.myMax;
	line.myToPosition.z = aBox.myMin.z;
	drawer.AddStaticLineCommand(line);
	line.myToPosition = aBox.myMax;
	line.myToPosition.x = aBox.myMin.x;
	drawer.AddStaticLineCommand(line);
	line.myToPosition = aBox.myMax;
	line.myToPosition.y = aBox.myMin.y;
	drawer.AddStaticLineCommand(line);

	line.myFromPosition = aBox.myMin;
	line.myFromPosition.x = aBox.myMax.x;
	line.myToPosition = aBox.myMax;
	line.myToPosition.y = aBox.myMin.y;
	drawer.AddStaticLineCommand(line);
	line.myFromPosition = aBox.myMin;
	line.myFromPosition.z = aBox.myMax.z;
	line.myToPosition = aBox.myMax;
	line.myToPosition.y = aBox.myMin.y;
	drawer.AddStaticLineCommand(line);

	line.myFromPosition = aBox.myMax;
	line.myFromPosition.x = aBox.myMin.x;
	line.myToPosition = aBox.myMin;
	line.myToPosition.y = aBox.myMax.y;
	drawer.AddStaticLineCommand(line);
	line.myFromPosition = aBox.myMax;
	line.myFromPosition.z = aBox.myMin.z;
	line.myToPosition = aBox.myMin;
	line.myToPosition.y = aBox.myMax.y;
	drawer.AddStaticLineCommand(line);


	line.myFromPosition = aBox.myMin;
	line.myFromPosition.x = aBox.myMax.x;
	line.myToPosition = aBox.myMax;
	line.myToPosition.z = aBox.myMin.z;
	drawer.AddStaticLineCommand(line);
	line.myFromPosition = aBox.myMin;
	line.myFromPosition.z = aBox.myMax.z;
	line.myToPosition = aBox.myMax;
	line.myToPosition.x = aBox.myMin.x;
	drawer.AddStaticLineCommand(line);


}

bool CanAttack(const CU::Vec3f& aPos, const CU::Vec3f& aOtherPos)
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
	ISTE::ComponentPool& attackValuePool = scene.GetComponentPool<ISTE::AttackValueComponent>();

	ISTE::TransformComponent* entityTransform;
	ISTE::ModelComponent* entityModel;

	//ISTE::DebugDrawer& drawer = ISTE::Context::Get()->myGraphicsEngine->GetDebugDrawer();

	//drawer.ClearStaticCommands();

	for (auto& entity : entities)
	{
		EntityIndex entIndex = GetEntityIndex(entity);

		if (!scene.IsEntityIDValid(entity) || (scene.GetEntity(entity).myMask & ISTE::Context::Get()->mySceneHandler->GetFBM()) != 0
			|| attackValuePool.Get(entIndex) != nullptr || scene.GetLayer(entity) == "IgnoreVisibility")
			continue;

		entityTransform = (ISTE::TransformComponent*)transformPool.Get(entIndex);
		entityModel = (ISTE::ModelComponent*)modelPool.Get(entIndex);

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
		{
			//temp
			CU::Box3D tempBox;
			tempBox.myMin = CU::Vec4f(recalcedMin.x, recalcedMin.y, recalcedMin.z, 1) * entityTransform->myCachedTransform;
			tempBox.myMax = CU::Vec4f(recalcedMax.x, recalcedMax.y, recalcedMax.z, 1) * entityTransform->myCachedTransform;
			DrawBox(tempBox);
			std::cout << scene.GetEntity(entity).myName << std::endl;
			//
			return false;
		}


		
	}

	return true;
}

void LookAt(ISTE::TransformComponent& aTransform, CU::Vec3f& aLookAtPos, float aLerpValue = 1)
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

ISTE::PlayerBehaviour::~PlayerBehaviour()
{
	myCtx->myEventHandler->RemoveCallback(EventType::PlayerGainExperience, "PlayerGainExperience");

	if (myBuildPathTimer)
		delete myBuildPathTimer;
	if (myMoveDistanceTimer)
		delete myMoveDistanceTimer;
}

void ISTE::PlayerBehaviour::Init()
{
	myCtx = ISTE::Context::Get();
	// Random Set
	myRandomEngine = std::mt19937(std::random_device()());
	myRandomYText = std::uniform_real_distribution<float>(-0.1f, 0.f);
	myRandomXText = std::uniform_real_distribution<float>(-0.2f, 0.f);

	InitAbilities();
	InitEvents();

	myIsDead = false;
	myHasAnOrder = false;
	myOnIFrameMode = false;

	//myCameraPositioning.myPosition = CU::Vec3f(-4.0f, 7.0f, 8.0f);
	//myCameraPositioning.myEuler.SetRotation(CU::Vec3f(40, 200.314f, -25));

	TransformComponent* transform = myCtx->Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myHostId);
	
	//if (myCtx->mySceneHandler->GetActiveSceneIndex() < 6) // out commented until we use static camera on boss level
	//{
	myCamera.GetTransformNonConst() = myCameraPositioning.myEuler.GetRotationMatrix();

	myCamera.GetTransformNonConst()(4, 1) = transform->myPosition.x + myCameraPositioning.myPosition.x;
	myCamera.GetTransformNonConst()(4, 2) = transform->myPosition.y + myCameraPositioning.myPosition.y;
	myCamera.GetTransformNonConst()(4, 3) = transform->myPosition.z + myCameraPositioning.myPosition.z;
	//}
	//else
	//{
	//	myCamera.GetTransformNonConst() = myCameraPositioning.myEuler.GetRotationMatrix();

	//	myCamera.GetTransformNonConst()(4, 1) = myCameraPositioning.myPosition.x;
	//	myCamera.GetTransformNonConst()(4, 2) = myCameraPositioning.myPosition.y;
	//	myCamera.GetTransformNonConst()(4, 3) = myCameraPositioning.myPosition.z;
	//}

	Context::Get()->myGraphicsEngine->SetCamera(myCamera);

	myBuildPathTimer = new LocalStepTimer();
	myBuildPathTimer->SetCallback([this]() { BuildPath(); });
	myBuildPathTimer->SetDelay(20);
	
	InitAnimationLerpSpeeds();
	InitAnimations();

	mySource = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<AudioSource>(myHostId);

	assert(mySource != nullptr && "AudioSource could not be found on player");

	UpdateWeapon();
	InitMoveDistanceTimer();
}

void ISTE::PlayerBehaviour::Update(float aDeltaTime)
{
	if (!myIsActive)
	{
		if (!myAnimationHelper.IsPlaying())
		{
			myAnimationHelper.PlayInterpelated(PlayerAnimations::eIdle, myAnimationLerpData.myAnyToIdleSpeed);
			mySource->Stop((int)PlayerSounds::eWalking);
		}
		return;
	}
	ISTE::Scene* scene = &myCtx->mySceneHandler->GetActiveScene();
	auto& transformPool = scene->GetComponentPool<TransformComponent>();
	TransformComponent* playerTransform = (TransformComponent*)transformPool.Get(GetEntityIndex(myHostId));
	CU::InputHandler* input = myCtx->myInputHandler;
	
	// set VFX position
	{
		VFXHandler* vfxH = myCtx->myVFXHandler;
		if (myMagicArmorVFX > -1)
			vfxH->SetVFXPosition(myMagicArmorVFX, playerTransform->myPosition);
		if (myAreaOfEffectVFX > -1)
			vfxH->SetVFXPosition(myAreaOfEffectVFX, playerTransform->myPosition);
		if (myLevelUpVFX > -1)
			vfxH->SetVFXPosition(myLevelUpVFX, playerTransform->myPosition);
		if (myPrimaryAttackVFX > -1)
		{
			vfxH->SetVFXPosition(myPrimaryAttackVFX, playerTransform->myPosition);
			vfxH->SetVFXRotation(myPrimaryAttackVFX, playerTransform->myEuler.GetAngles());
			// positioing vfx to axe blade bone
			{
				//TransformComponent* weaponT = (TransformComponent*)transformPool.Get(GetEntityIndex(myWeaponId));
				//ModelComponent* weaponM = scene->GetComponent<ModelComponent>(myWeaponId);
				//Model* weaponModel = Context::Get()->myModelManager->GetModel(weaponM->myModelId);

				//int boneId = weaponModel->myBoneNameToId["TopJoint_Axe_SK"];
				//CU::Matrix4x4f invereseWeaponMatrix = weaponModel->myBones[boneId].myBindToBone;

				////CU::Matrix4x4f weaponThisFrame = CU::Matrix4x4f::CreateScaleMatrix(weaponT->myScale) * weaponT->myQuaternion.GetRotationMatrix4X4() * CU::Matrix4x4f::CreateTranslationMatrix(weaponT->myPosition);
				//CU::Matrix4x4f tempMatrix = (invereseWeaponMatrix * CU::Matrix4x4f::CreateRotationAroundX(260 * DegToRad)) * weaponT->myCachedTransform;

				//myCtx->myVFXHandler->SetVFXPosition(myPrimaryAttackVFX,  tempMatrix.GetTranslationV3());
			}
		}
	}

	if (myIsDead)
	{
		myAnimationHelper.PlayInterpelated(PlayerAnimations::eDead, myAnimationLerpData.myAnyToDeadSpeed);
		
		

		if (!myAnimationHelper.IsPlaying())
		{
			//Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eWTTBP);
			//Context::Get()->mySceneHandler->LoadScene(0);

		}
		// Add timer to fadeout, things that happens once when player dies
		static bool deathTimerAdded;

		if(!deathTimerAdded)
			mySource->Play((int)PlayerSounds::eDeath, ASP_EXLUSIVE | ASP_IGNOREIFACTIVE);

		myAnimationHelper.Update();
		UpdateWeapon();


		myDeathTimer += aDeltaTime; 
		float t = 1 - myDeathTimer/3.f;
		Context::Get()->myGraphicsEngine->GetAberrationEffect().GetBufferData().myRadialStrength = { t,0,0 };
		if (deathTimerAdded) return;
		myCtx->myEventHandler->InvokeEvent(EventType::PlayerDied, 0);
		CountDown timer;
		timer.name = "Lose Timer";
		timer.duration = 2.f; // should be time it takes for player to fall to ground
		timer.callback = []() { 
			Context::Get()->myGraphicsEngine->GetAberrationEffect().SetEnabled(false); 
			Context::Get()->myGenericDatabase->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "Lose Screen");
			Context::Get()->myStateManager->PushState(ISTE::StateType::FadeOut);
			deathTimerAdded = false;
		};
		myCtx->myTimeHandler->AddTimer(timer);
		
		myCtx->myGenericDatabase->SetValue<int>("DeathScreen_Emitter_1", myCtx->mySprite2DParticleHandler->SpawnEmitter("DeathScreen_Dust_Red_Small"));
		myCtx->myGenericDatabase->SetValue<int>("DeathScreen_Emitter_2", myCtx->mySprite2DParticleHandler->SpawnEmitter("DeathScreen_Dust_Red_Big"));
		myCtx->myGenericDatabase->SetValue<int>("DeathScreen_Emitter_3", myCtx->mySprite2DParticleHandler->SpawnEmitter("DeathScreen_Dust_White_Small"));
		myCtx->myGenericDatabase->SetValue<int>("DeathScreen_Emitter_4", myCtx->mySprite2DParticleHandler->SpawnEmitter("DeathScreen_Dust_White_Big"));

		deathTimerAdded = true;

		return;
	}

	if (input->IsKeyDown('T'))
	{
		myAnimationHelper.PlayInterpelated(PlayerAnimations::ePlayerBag, myAnimationLerpData.myAnyToBagSpeed);
		myCtx->myModelVFXHandler->Spawn("Player_Ultimate_Axe", playerTransform->myPosition);
		CountDown timer;
		timer.callback = [ pos = playerTransform->myPosition]() {
			Context::Get()->myVFXHandler->SpawnVFX("Player_Ultimate", pos);
		};
		timer.duration = 0.7f;
		myCtx->myTimeHandler->AddTimer(timer);
		mySource->Play((int)PlayerSounds::ePrimaryAttack);
		mySource->SetPitch((int)PlayerSounds::ePrimaryAttack, CU::GetRandomFloat(0.8f, 1.2f));
	}
	if (myHealth <= myHealthMax / 3.f)
	{
		float t = 1 - (myHealth * 3.f / myHealthMax) / 1.1f;

		myCtx->myGraphicsEngine->GetAberrationEffect().SetEnabled(true);
		myCtx->myGraphicsEngine->GetAberrationEffect().SetType(ChromaticAberrationEffect::Type::eRadial);
		myCtx->myGraphicsEngine->GetAberrationEffect().GetBufferData().myRadialStrength = { t ,0,0 };
	}
	else
		myCtx->myGraphicsEngine->GetAberrationEffect().SetEnabled(false);
	if (myHealth <= 0)
	{ 
		myIsDead = true;
		return;
	}

#ifdef _DEBUG // able to move holding ctrl
	bool ctrl = input->IsKeyHeldDown(VK_CONTROL);
	if (input->IsKeyDown(MK_LBUTTON) && ctrl)
	{
		ScreenHelper::GPUPickedData result = myScreenHelper.Pick(myScreenHelper.GetMouseRelativePosition());

		EntityID ide = scene->GetAssociatedID((EntityIndex)result.myEntityId);

		myOrders.push(result.myPosition);
		myHasAnOrder = true;
	}
#endif // _DEBUG

	//if (!myAnimationHelper.IsPlaying())
	//{
	//	myAnimationHelper.PlayInterpelated(PlayerAnimations::eIdle, myAnimationLerpData.myAnyToIdleSpeed);
	//	//mySource->Stop();
	//	mySource->Stop((int)PlayerSounds::eWalking);
	//}

	if (!myIsStaled)
	{
		Move(aDeltaTime);
	}
	else
	{
		myStalTimer += aDeltaTime;
		if (myStalTimer >= myStalTime)
			myIsStaled = false;
	}

	if (input->IsKeyDown(MK_LBUTTON))
		BuildPath();
	myBuildPathTimer->Update(aDeltaTime);
	myMoveDistanceTimer->Update(aDeltaTime);

	bool shiftFlag = myCtx->myInputHandler->IsKeyHeldDown(VK_SHIFT);
	if (shiftFlag)
	{
		//Reset(); // Reset path to stop player when using melee primary attack

		ScreenHelper::GPUPickedData result = myScreenHelper.Pick(myScreenHelper.GetMouseRelativePosition());
		LookAt(*playerTransform, result.myPosition, 20.f * aDeltaTime);
	}
	if (input->IsKeyHeldDown(MK_LBUTTON) && myCanUsePrimary)
	{
		//might need a range check
		EntityID ide = myCtx->mySystemManager->GetSystem<EnemyBehaviourSystem>()->GetEntityIDFromScreen();
		EnemyStatsComponent* stats = scene->GetComponent<EnemyStatsComponent>(ide);
	

		if (shiftFlag)
		{
			DoAbility(ISTE::PlayerAbility::ePrimary);
		}
		else if (ide != INVALID_ENTITY)
		{
			if ((stats != nullptr && !stats->myIsDead)) // || stats == nullptr -- if we want to be able to hit pots with primary
			{
				TransformComponent* enemyComp = scene->GetComponent<TransformComponent>(ide);
				TriggerComponent* enemytrigg = scene->GetComponent<TriggerComponent>(ide);
				SphereTriggerComponent* sphereEnemyTrigg = scene->GetComponent<SphereTriggerComponent>(ide);

				CU::Vec3f offset;

				if (enemytrigg != nullptr)
					offset = enemytrigg->myOffset;
				else if (sphereEnemyTrigg != nullptr)
					offset = sphereEnemyTrigg->myOffset;

				bool inRangeFlag = ((enemyComp->myPosition + offset) - playerTransform->myPosition).Length() <= myPrimaryAttackRange;
				if(inRangeFlag)
					DoAbility(ISTE::PlayerAbility::ePrimary);
			}
		}
	}

	if (input->IsKeyDown(MK_RBUTTON))
	{
		DoAbility(ISTE::PlayerAbility::eSecondary);
	}

	//if (input->IsKeyDown('Z') && myCanUsePrimary)
	//	DoAbility(ISTE::PlayerAbility::ePrimary);
	//if (input->IsKeyDown('X'))
	//	DoAbility(ISTE::PlayerAbility::eSecondary);

	if (input->IsKeyDown('1') || input->IsKeyDown('A'))
		DoAbility(ISTE::PlayerAbility::eMagicArmor);
	if (input->IsKeyDown('2') || input->IsKeyDown('S'))
		DoAbility(ISTE::PlayerAbility::eAOEAtPlayer);
	if (input->IsKeyDown('3') || input->IsKeyDown('D'))
		DoAbility(ISTE::PlayerAbility::eTeleport);
	if (input->IsKeyDown('4') || input->IsKeyDown('F'))
		DoAbility(ISTE::PlayerAbility::eAOEAtTarget);

	// Regeneration
	{
		//if (myHealth < myHealthMax)
		//{
		//	if (myHealth + myHealthReg * aDeltaTime >= myHealthMax)
		//		myHealth = myHealthMax;
		//	else
		//		myHealth += myHealthReg * aDeltaTime;
		//} 

		//if (myMana < myManaMax)
		//{
		//	if (myMana + myManaReg * aDeltaTime >= myManaMax)
		//		myMana = myManaMax;
		//	else
		//		myMana += myManaReg * aDeltaTime;
		//}

		myHealth = CU::Min(myHealth + myHealthReg * aDeltaTime, myHealthMax);
		myMana	 = CU::Min(myMana	+ myManaReg   * aDeltaTime, myManaMax);

	}

	//
	Context::Get()->myGraphicsEngine->SetCamera(myCamera);

	if (myCtx->mySceneHandler->GetActiveSceneIndex() < 6)
	{
		myCamera.GetTransformNonConst() = myCameraPositioning.myEuler.GetRotationMatrix();

		myCamera.GetTransformNonConst()(4, 1) = playerTransform->myPosition.x + myCameraPositioning.myPosition.x;
		myCamera.GetTransformNonConst()(4, 2) = playerTransform->myPosition.y + myCameraPositioning.myPosition.y;
		myCamera.GetTransformNonConst()(4, 3) = playerTransform->myPosition.z + myCameraPositioning.myPosition.z;
	}

	myAnimationHelper.Update();
	UpdateWeapon();
}

void ISTE::PlayerBehaviour::OnTrigger(EntityID aId)
{
	AttackValueComponent* attacked = myCtx->mySceneHandler->GetActiveScene().GetComponent<AttackValueComponent>(aId);
	if (attacked == nullptr || attacked->myTag != "EnemyAttack" || myOnIFrameMode) // Haha, not attacked this time.
		return;

	if (attacked->myExtraInfo == "Projectile")
	{
		Context::Get()->myTimeHandler->InvokeTimer("HunterAttack_" + attacked->myIdentifier);
		Context::Get()->myTimeHandler->RemoveTimer("HunterAttack_" + attacked->myIdentifier);
		Context::Get()->myTimeHandler->RemoveTimer("HunterProjectileTimer_" + attacked->myIdentifier);
	}

	if (myArmored)
		return;

	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ModelComponent* model = scene.GetComponent<ModelComponent>(myHostId);
	model->myColor += CU::Vec3f(3.f, 3.f, 3.f);

	// I Frame Timer
	myOnIFrameMode = true;
	CountDown iFrameTimer;
	iFrameTimer.name = "PlayerIFrameTimer";
	iFrameTimer.duration = 0.2f;
	iFrameTimer.callback = [this, model]() {
		myOnIFrameMode = false;
		model->myColor -= CU::Vec3f(3.f, 3.f, 3.f);
	};
	Context::Get()->myTimeHandler->AddTimer(iFrameTimer);

	myHealth -= attacked->myAttackValue;


	std::uniform_real_distribution<float> dist(0.8f, 1.2f);
	std::random_device rd;

	myCtx->myEventHandler->InvokeEvent(EventType::PlayerTookDamage, INVALID_ENTITY);
	mySource->Play((int)PlayerSounds::eTookDamage);
	mySource->SetPitch((int)PlayerSounds::eTookDamage, dist(rd));

	PrintDamageText(attacked->myAttackValue);

	if (myHealth <= 0.f) {
		 
		myIsDead = true;

	}
}

void ISTE::PlayerBehaviour::DoAbility(PlayerAbility anAbility)
{
	std::string consoleOutput;
	switch (anAbility)
	{
	case ISTE::PlayerAbility::ePrimary:
		PrimaryAttack();
		break;
	case ISTE::PlayerAbility::eSecondary:
		SecondaryAttack();
		break;
	case ISTE::PlayerAbility::eMagicArmor:
		if (!myArmorUnlocked) break;
		MagicArmor();
		break;
	case ISTE::PlayerAbility::eTeleport:
		if (!myTeleportUnlocked) break;
		Teleport();
		break;
	case ISTE::PlayerAbility::eAOEAtPlayer:
		if (!myAoEDoTUnlocked) break;
		AoEDoT();
		break;
	case ISTE::PlayerAbility::eAOEAtTarget:
		if (!myUltimateUnlocked) break;
		Ultimate();
		break;
	case ISTE::PlayerAbility::eCount:
		break;
	default:
		break;
	}
}

void ISTE::PlayerBehaviour::Move(float aDeltaTime)
{ 
	if (myOrders.empty() || !myHasAnOrder)
	{
		/*Context::Get()->mySceneHandler->GetActiveScene().GetComponent<AnimatorComponent>(myHostId)->myCurrentAnimation = myIdleId;*/
		if (myAnimationHelper.GetCurrentMap() == PlayerAnimations::eMovement)
		{
			myAnimationHelper.PlayInterpelated(PlayerAnimations::eIdle, myAnimationLerpData.myMoveToIdleSpeed, PlayerAnimations::eMovement);
		}
		else if (myAnimationHelper.GetCurrentMap() != PlayerAnimations::eThrowAbility && myAnimationHelper.GetLastMap() == PlayerAnimations::eMovement && myAnimationHelper.GetCurrentMap() != PlayerAnimations::eIdle)
		{
			ModelComponent* mC = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<ModelComponent>(myHostId);
			int partialJoint = Context::Get()->myModelManager->GetBoneNameToIdMap(mC->myModelId)["Spine1_SK"];
			myAnimationHelper.PlayPartially(PlayerAnimations::eIdle, myAnimationHelper.GetCurrentMap(), partialJoint, 0.95, true);
		}
		else
			myAnimationHelper.Play(PlayerAnimations::eIdle);

		mySource->Stop((int)PlayerSounds::eWalking);

		return;
	}

	/*Context::Get()->mySceneHandler->GetActiveScene().GetComponent<AnimatorComponent>(myHostId)->myCurrentAnimation = myWalkId;*/ 
	if (myAnimationHelper.GetCurrentMap() == PlayerAnimations::eIdle)
	{
		myAnimationHelper.PlayInterpelated(PlayerAnimations::eMovement, myAnimationLerpData.myIdleToMoveSpeed);
	}
	else if (myAnimationHelper.GetCurrentMap() == PlayerAnimations::ePlayerBag)
	{
		myAnimationHelper.PlayInterpelated(PlayerAnimations::ePlayerBag, 0.25);
	}
	else if (myAnimationHelper.GetLastMap() == PlayerAnimations::eIdle && myAnimationHelper.GetCurrentMap() != PlayerAnimations::eMovement)
	{
		ModelComponent* mC = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<ModelComponent>(myHostId);
		int partialJoint = Context::Get()->myModelManager->GetBoneNameToIdMap(mC->myModelId)["Spine1_SK"];
		myAnimationHelper.PlayPartially(PlayerAnimations::eMovement, myAnimationHelper.GetCurrentMap(), partialJoint, 0.95, true);
	}
	else if (myAnimationHelper.IsPlaying() && myAnimationHelper.GetLastMap() == PlayerAnimations::eMovement && myAnimationHelper.GetLastMap() != PlayerAnimations::eIdle)
	{
		ModelComponent* mC = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<ModelComponent>(myHostId);
		int partialJoint = Context::Get()->myModelManager->GetBoneNameToIdMap(mC->myModelId)["Spine1_SK"];
		myAnimationHelper.PlayPartially(PlayerAnimations::eMovement, myAnimationHelper.GetCurrentMap(), partialJoint, 0.95, true);
	}
	else
		myAnimationHelper.Play(PlayerAnimations::eMovement);

	//if (mySource->GetActiveClip() != (int)PlayerSounds::eWalking || !mySource->IsPlaying())
	//{
	//	mySource->SetActiveClip((int)PlayerSounds::eWalking);
	//	mySource->Play();
	//}

	mySource->Play((int)PlayerSounds::eWalking, ASP_IGNOREIFACTIVE);

	TransformComponent* transform = myCtx->Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myHostId);
	CU::Vec3f point = myOrders.front();

	float length;
	CU::Vec3f unitVector = MovementMath::GetUnitVector3(transform->myPosition, point, length);


	if (length < 0.1f)
	{
		transform->myPosition = point;
		myOrders.pop();
		if (myOrders.empty())
			myHasAnOrder = false;
	}
	else
	{
		LookAt(*transform, point, 20.f * aDeltaTime);


		transform->myPosition = transform->myPosition + unitVector * aDeltaTime * mySpeed;
	}
	if (myDoTUsed)
	{
		myCtx->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myAttacksIds["AoEDoT"])->myPosition = transform->myPosition;
	}

}

void ISTE::PlayerBehaviour::BuildPath()
{
	auto input = myCtx->myInputHandler;
	bool leftHeldDown = input->IsKeyHeldDown(MK_LBUTTON);
	bool middleHeldDown = input->IsKeyHeldDown(255);

	if (input->IsKeyHeldDown(VK_SHIFT))
	{
		Reset();
		return;
	}
	if (!leftHeldDown && !middleHeldDown)
		return;

 	if (myHasAnOrder)
		Reset();

	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();

	// Get
	EntityID enemyId = myCtx->mySystemManager->GetSystem<EnemyBehaviourSystem>()->GetEntityIDFromScreen();
	EnemyStatsComponent* enemyStats = scene.GetComponent<EnemyStatsComponent>(enemyId);
	TransformComponent* playerTransform = myCtx->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myHostId);
	TransformComponent* enemyTransform = myCtx->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(enemyId);
	if (enemyStats != nullptr && !enemyStats->myIsDead)
	{
		float length = (playerTransform->myPosition - enemyTransform->myPosition).Length();
		if (length <= myPrimaryAttackRange)
		{ 
			LookAt(*playerTransform, enemyTransform->myPosition,0.4f);
			return;
		}
	}

	//Set
	const CU::Vec3f& start = playerTransform->myPosition;
	ScreenHelper::Ray ray = myScreenHelper.GetRayFromPoint(myScreenHelper.GetMouseRelativePosition(), myCtx->myWindow->GetResolution().myValue);
	CU::Vec3f destination;
	
	const bool destinationFoundInNavMesh = NavMesh::RayIntersectsNavMesh(ray.myOrigin, ray.myDir, destination);

	if ((start - destination).Length() <= 0.2f)
		return;

 	if (!destinationFoundInNavMesh)
	{
		myOrders = std::queue<CU::Vec3f>();
		float nonResponsiveAngle = 1;
		float maxDist = 40.f;

		if (Context::Get()->mySceneHandler->GetActiveSceneIndex() < 5)
		{
			maxDist = 20.f;
			nonResponsiveAngle = 5;
		}

		// Get a destination point at the edge of NavMesh
		//NavMesh::RayIntersectionOff(ray.myOrigin, ray.myDir, nonResponsiveAngle, start, destination);
		if (NavMesh::RayClosestPointIntersection(ray.myOrigin, ray.myDir, nonResponsiveAngle, start, destination, maxDist))
		{
			NavMesh::FindPointPath(start, destination, myOrders);

			if (myOrders.size() == 1 && myOrders.front() == start)
			{

				std::vector<NavMesh::Intersection> inters;
				NavMesh::GetIntersections(scene.GetNavMesh(), start, destination, inters);

				bool foundPoint = false;
				for (auto& inter : inters)
				{
					for (auto& sect : inter.myIntersections)
					{
						myOrders = std::queue<CU::Vec3f>();
						NavMesh::FindPointPath(start, sect, myOrders);

						if (myOrders.size() != 1 || myOrders.front() != start)
						{
							foundPoint = true;
							break;
						}
					}

					if (foundPoint)
						break;
				}

				if (!foundPoint)
				{
					myOrders = std::queue<CU::Vec3f>();
					return;
				}

				if (input->IsKeyDown(MK_LBUTTON) || input->IsKeyDown(255)) // only spawn one path indicator
					myCtx->myVFXHandler->SpawnVFX("Player_Path_Indicator", myOrders.back());

				myHasAnOrder = true;
				return;
			}

			if ((start - destination).Length() <= 0.2f)
			{
				myOrders = std::queue<CU::Vec3f>();
				return;
			}
			if (input->IsKeyDown(MK_LBUTTON) || input->IsKeyDown(255)) // only spawn one path indicator
				myCtx->myVFXHandler->SpawnVFX("Player_Path_Indicator", myOrders.back());

			myHasAnOrder = true;
		}

		return;

	}

	NavMesh::FindPointPath(start, destination, myOrders);

	if (myOrders.size() == 1 && myOrders.front() == start)
	{
		myOrders = std::queue<CU::Vec3f>();
		float nonResponsiveAngle = 5;
		// Get a destination point at the edge of NavMesh
		//NavMesh::RayIntersectionOff(ray.myOrigin, ray.myDir, nonResponsiveAngle, start, destination);
		if (NavMesh::RayClosestPointIntersection(ray.myOrigin, ray.myDir, nonResponsiveAngle, start, destination, 20.f))
		{
			NavMesh::FindPointPath(start, destination, myOrders);

			if (myOrders.size() == 1 && myOrders.front() == start)
			{

				std::vector<NavMesh::Intersection> inters;
				NavMesh::GetIntersections(scene.GetNavMesh(), start, destination, inters);

				bool foundPoint = false;
				for (auto& inter : inters)
				{
					for (auto& sect : inter.myIntersections)
					{
						myOrders = std::queue<CU::Vec3f>();
						NavMesh::FindPointPath(start, sect, myOrders);

						if (myOrders.size() != 1 || myOrders.front() != start)
						{
							foundPoint = true;
							break;
						}
					}

					if (foundPoint)
						break;
				}

				if (!foundPoint)
				{
					myOrders = std::queue<CU::Vec3f>();
					return;
				}

				if (input->IsKeyDown(MK_LBUTTON) || input->IsKeyDown(255)) // only spawn one path indicator
					myCtx->myVFXHandler->SpawnVFX("Player_Path_Indicator", myOrders.back());

				myHasAnOrder = true;
				return;
			}

			if ((start - destination).Length() <= 0.2f)
			{
				myOrders = std::queue<CU::Vec3f>();
				return;
			}
			if (input->IsKeyDown(MK_LBUTTON) || input->IsKeyDown(255)) // only spawn one path indicator
				myCtx->myVFXHandler->SpawnVFX("Player_Path_Indicator", myOrders.back());

			myHasAnOrder = true;
		}

		return;
	}

	//lazy
	std::queue<CU::Vec3f> ordersCopy = myOrders;

	float totalDist = 0;
	CU::Vec3f lastDestPoint = start;
	CU::Vec3f destPoint;
	while (!ordersCopy.empty())
	{
		destPoint = ordersCopy.front();
		ordersCopy.pop();

		totalDist += (lastDestPoint - destPoint).Length();
		lastDestPoint = destPoint;

		//max dist val
		if (totalDist >= 50.f)
		{
			myOrders = std::queue<CU::Vec3f>();
			float nonResponsiveAngle = 0;

			if (NavMesh::RayClosestPointIntersection(ray.myOrigin, ray.myDir, nonResponsiveAngle, start, destination, 20.f))
			{
				NavMesh::FindPointPath(start, destination, myOrders);

				if ((start - destination).Length() <= 0.2f)
				{
					myOrders = std::queue<CU::Vec3f>();
					return;
				}

				if (input->IsKeyDown(MK_LBUTTON) || input->IsKeyDown(255)) // only spawn one path indicator
					myCtx->myVFXHandler->SpawnVFX("Player_Path_Indicator", myOrders.back());

				myHasAnOrder = true;
			}
			return;
		}
	}

	if (input->IsKeyDown(MK_LBUTTON) || input->IsKeyDown(255)) // only spawn one path indicator
		myCtx->myVFXHandler->SpawnVFX("Player_Path_Indicator", myOrders.back());

	if (myOrders.front() == start && myOrders.size() <= 1)
		return;

	if (myOrders.back() == start)
		return;

	myHasAnOrder = true;
}

void ISTE::PlayerBehaviour::Reset()
{
	myHasAnOrder = false;
	std::queue<CU::Vec3f> empty;
	std::swap(myOrders, empty);
}

void ISTE::PlayerBehaviour::InitAbilities()
{
	// Json import data //Ability 1,3,4 is left.

	CU::Database<true>& behaviours = myCtx->mySceneHandler->GetActiveScene().GetBehaviourDatabase();

	mySpeed = behaviours.Get<float>("PlayerSpeed");
	myHealthMax = behaviours.Get<float>("PlayerHealth");
	myHealthReg = behaviours.Get<float>("PlayerHealthreg");
	myManaMax = behaviours.Get<float>("PlayerMana");
	myManaReg = behaviours.Get<float>("PlayerManareg");

	myHealth = myHealthMax;
	myMana = myManaMax;

	myExperienceRequired = behaviours.Get<int>("PlayerReqExp");

	// Primary
	myPrimaryDamage = behaviours.Get<float>("PlayerPriDamage");
	myPrimaryAttackRange = behaviours.Get<float>("PlayerPriRange");
	myPrimaryAttackSpeed = behaviours.Get<float>("PlayerPriAttackspeed");
	myPrimaryAttackStayrate = behaviours.Get<float>("PlayerPriStayrate");
	myCanUsePrimary = true;

	//secondary
	mySecondaryDamage = behaviours.Get<float>("PlayerSecDamage");
	mySecondaryAttackStay = behaviours.Get<float>("PlayerSecStay");
	mySecondaryAttackMovmentSpeed = behaviours.Get<float>("PlayerSecMoveSpeed");
	mySecondaryAttackCost = behaviours.Get<float>("PlayerSecCost");
	myStalTime = behaviours.Get<float>("PlayerSecStalTime");

	// Magic Armor

	myArmorCooldown = behaviours.Get<float>("PlayerArmCooldown");
	myArmorCost = behaviours.Get<float>("PlayerArmCost");
	myArmorDuration = behaviours.Get<float>("PlayerArmDuration");

	// Teleport ability

	myTeleportCooldown = behaviours.Get<float>("PlayerTelCooldown");
	myTeleportCost = behaviours.Get<float>("PlayerTelCost");
	myMaxTeleportDistance = behaviours.Get<float>("PlayerTelDistance");
	myCanUseTeleport = true;

	// AoEDoT
	myAoEDoTCooldown = behaviours.Get<float>("PlayerAoeCooldown");
	myAoEDoTCost = behaviours.Get<float>("PlayerAoeCost");
	myAoEDoTDamage = behaviours.Get<float>("PlayerAoeDamage");
	myAoEDoTDuration = behaviours.Get<float>("PlayerAoeDuration");
	myAoEDoTRadius = behaviours.Get<float>("PlayerAoeRadius");

	// Ultimate

	myUltimateCooldown = behaviours.Get<float>("PlayerUltCooldown");
	myUltimateCost = behaviours.Get<float>("PlayerUltCost");
	myUltimateDamage = behaviours.Get<float>("PlayerUltDamage");
	myUltimateRadius = behaviours.Get<float>("PlayerUltRadius");
	myUltimateRange = behaviours.Get<float>("PlayerUltRange");
	myUltimateHealthRegen = behaviours.Get<float>("PlayerUltRegen");

	// Unlock abilites based on current level
	const int sceneIndex = myCtx->mySceneHandler->GetActiveSceneIndex();

	if (sceneIndex >= 2)
	{
		myArmorUnlocked	= true;
	}
	if (sceneIndex >= 3)
	{
		myAoEDoTUnlocked = true;
	}
	if (sceneIndex >= 4)
	{
		myTeleportUnlocked = true;
	}
	if (sceneIndex >= 5)
	{
		myUltimateUnlocked = true;
	}
}

void ISTE::PlayerBehaviour::InitEvents()
{
	if (myCtx->mySceneHandler->GetActiveSceneIndex() < 6) // if not boss level
		myCtx->myEventHandler->RegisterCallback(ISTE::EventType::PlayerGainExperience, "PlayerGainExperience", [this](EntityID someExperience) { GainExperience((int)someExperience); });
}

void ISTE::PlayerBehaviour::InitAnimations()
{
	myAnimationHelper.SetEntityID(myHostId);
	myAnimationHelper.MapAnimation(PlayerAnimations::eIdle, myIdleId, AH_LOOPING, 0, 0);
	myAnimationHelper.MapAnimation(PlayerAnimations::eMovement, myWalkId, AH_LOOPING, 0, 0);
	myAnimationHelper.MapAnimation(PlayerAnimations::eDead, myDeadId, 0, 2, 2);
	myAnimationHelper.MapAnimation(PlayerAnimations::eThrowAbility, myThrowAbilityId, 0, 1, 1);
	myAnimationHelper.MapAnimation(PlayerAnimations::eMeleeAttack, myMeleeAttackId, 0, 1, 1);
	myAnimationHelper.MapAnimation(PlayerAnimations::eSpell, mySpellId, 0, 1, 1);
	myAnimationHelper.MapAnimation(PlayerAnimations::ePlayerBag, Context::Get()->myAnimationManager->LoadAnimation(myHostId, "../Assets/Animations/CH_Player_crouch_ANIM.fbx").myValue, 0, 3, 0);
}

void ISTE::PlayerBehaviour::InitAnimationLerpSpeeds()
{
	CU::Database<true>& behaviours = myCtx->mySceneHandler->GetActiveScene().GetBehaviourDatabase();

	myAnimationLerpData.myMoveToIdleSpeed	= behaviours.Get<float>("PlayerMoveToIdleSpeed");
	myAnimationLerpData.myIdleToMoveSpeed	= behaviours.Get<float>("PlayerIdleToMoveSpeed");

	myAnimationLerpData.myAnyToMoveSpeed			= behaviours.Get<float>("PlayerAnyToMoveSpeed");
	myAnimationLerpData.myAnyToIdleSpeed			= behaviours.Get<float>("PlayerAnyToIdleSpeed");
	myAnimationLerpData.myAnyToPrimaryAttackSpeed	= behaviours.Get<float>("PlayerAnyToPrimaryAttackSpeed");
	myAnimationLerpData.myAnyToSecondaryAttackSpeed = behaviours.Get<float>("PlayerAnyToSecondaryAttackSpeed");
	myAnimationLerpData.myAnyToSpellSpeed			= behaviours.Get<float>("PlayerAnyToSpellSpeed");
	myAnimationLerpData.myAnyToDeadSpeed			= behaviours.Get<float>("PlayerAnyToDeadSpeed");
	myAnimationLerpData.myAnyToBagSpeed				= behaviours.Get<float>("PlayerAnyToBagSpeed");

}

void ISTE::PlayerBehaviour::PrintDamageText(float aDamageValue)
{
	// Get Delta Time and active scene
	float deltaTime = myCtx->myTimeHandler->GetDeltaTime();
	Scene* scene = &myCtx->mySceneHandler->GetActiveScene();

	// Get Ratio
	const CU::Vec2Ui res = myCtx->myWindow->GetResolution();
	const float ratio = (float)res.x / res.y;

	// Create new entity
	myDamageTextCount += 1;

	std::string id = "playerDamageText" + std::to_string(myDamageTextCount);
	myDamageTextIds[id] = scene->NewEntity();

	// Assign transfrom component
	TransformComponent* ptransform = myCtx->Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myHostId);
	TransformComponent* transform = scene->AssignComponent<TransformComponent>(myDamageTextIds[id]);
	transform->myPosition = CU::Vec3f(myRandomXText(myRandomEngine), myRandomYText(myRandomEngine), 0.f);
	transform->myScale = CU::Vec3f(1.f, 1.f, 0.f);
	transform->myScale.y *= ratio;

	// Assign text component
	TextComponent* text = scene->AssignComponent<TextComponent>(myDamageTextIds[id]);
	text->myFontID = myCtx->myTextureManager->LoadFont("../Assets/Font/arial.ttf", 16);
	text->myText = "-" + std::to_string((int)aDamageValue) + "hp";
	text->myPivot = { 0.f, 0.f };
	text->myScale = 1.f;
	text->myColor = { 1.f, 0.f, 0.f, 1.f };

	// Timer values stuff
	float duration = 5.f;

	// Timer for new entity : deletion
	CountDown textStayTimer;
	textStayTimer.name = "playerDamageStayTextTimer";
	textStayTimer.duration = duration;
	textStayTimer.callback = [this, id]() { Context::Get()->mySceneHandler->GetActiveScene().DestroyEntity(myDamageTextIds[id]); };
	myCtx->myTimeHandler->AddTimer(textStayTimer);

	// Timer for new entity : movement
	UpdateTimer textTimer;
	textTimer.name = "playerDamageTextTimer";
	textTimer.duration = duration - 0.01f;
	textTimer.callback = [this, deltaTime, text, transform]() {
		transform->myPosition = {
			transform->myPosition.x,
			transform->myPosition.y + 0.1f * deltaTime,
			transform->myPosition.z
		};

		text->myColor.w = text->myColor.w - 0.1f * deltaTime;
	};
	myCtx->myTimeHandler->AddTimer(textTimer);
}

void ISTE::PlayerBehaviour::GainExperience(const int someExperience)
{
	if (myHaveGainedLevel)
		return;

	myExperience += someExperience;

	if (myExperienceRequired <= myExperience)
	{
		myHaveGainedLevel = true;
		//mySource->SetActiveClip((int)PlayerSounds::eLevelUp);
		//mySource->Play();
		
		mySource->Play((int)PlayerSounds::eLevelUp);

		ComponentPool& transformPool = myCtx->mySceneHandler->GetActiveScene().GetComponentPool<TransformComponent>();
		TransformComponent* transform = (TransformComponent*)transformPool.Get(GetEntityIndex(myHostId));
		myLevelUpVFX = myCtx->myVFXHandler->SpawnVFX("Player_LevelUp", transform->myPosition);

		// Unlock ability based on current level
		const int sceneIndex = Context::Get()->mySceneHandler->GetActiveSceneIndex();
		switch (sceneIndex)
		{
		case 1:
			myArmorUnlocked = true;
			break;
		case 2:
			myAoEDoTUnlocked = true;
			break;
		case 3:
			myTeleportUnlocked = true;
			break;
		case 4:
			myUltimateUnlocked = true;
			break;
		}
		// Send event that player dinged
		myHealth = myHealthMax;
		myMana = myManaMax;
		Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::PlayerGainLevel, sceneIndex);
		Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::PlayerTookDamage, sceneIndex);
		Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::PlayerSpentMana, sceneIndex);
	}
}

void ISTE::PlayerBehaviour::UpdateWeapon()
{
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	ComponentPool& transformPool = scene.GetComponentPool<TransformComponent>();
	TransformComponent* playerT = (TransformComponent*)transformPool.Get(GetEntityIndex(myHostId));
	TransformComponent* weaponT = (TransformComponent*)transformPool.Get(GetEntityIndex(myWeaponId));
	AnimatorComponent* animator = myAnimationHelper.GetAnimatorComp();

	ModelComponent* weaponM = scene.GetComponent<ModelComponent>(myWeaponId);
	Model* weaponModel = Context::Get()->myModelManager->GetModel(weaponM->myModelId);

	if (animator == nullptr)
		return;


	CU::Matrix4x4f invereseWeaponMatrix = weaponModel->myBones[myWeaponHoldBone].myBindToBone;
	CU::Matrix4x4f handDumb = animator->myPose.myModelSpaceBoneTransforms[myRightHandBoneIndex];
	CU::Matrix4x4f playerThisFrame = CU::Matrix4x4f::CreateScaleMatrix(playerT->myScale) * playerT->myQuaternion.GetRotationMatrix4X4() * CU::Matrix4x4f::CreateTranslationMatrix(playerT->myPosition);
	CU::Matrix4x4f tempMatrix = invereseWeaponMatrix * handDumb * playerThisFrame;

	weaponT->myPosition = tempMatrix.GetTranslationV3();

	CU::Matrix3x3f tempRot(tempMatrix);
	CU::Vec3f scale = tempMatrix.DecomposeScale();
	tempRot.GetRow(1) = tempRot.GetRow(1) / scale.x;
	tempRot.GetRow(2) = tempRot.GetRow(2) / scale.y;
	tempRot.GetRow(3) = tempRot.GetRow(3) / scale.z;

	CU::Quaternionf tor = CU::Quaternionf(tempRot);
	weaponT->myQuaternion = tor;
	weaponT->myEuler.ToEulerAngles();
}

void ISTE::PlayerBehaviour::PrimaryAttack()
{
	// Reset path to stop player when using melee primary attack
	Reset();

	//mySource->SetActiveClip((int)PlayerSounds::ePrimaryAttack, false);
	//mySource->Play();

	// send event
	Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::PlayerUsedAbility, 0);

	std::uniform_real_distribution<float> dist(0.8f, 1.2f);
	std::random_device rd;

	mySource->Play((int)PlayerSounds::ePrimaryAttack);
	mySource->SetPitch((int)PlayerSounds::ePrimaryAttack, dist(rd));

	

	if (myAnimationHelper.GetCurrentMap() == PlayerAnimations::eIdle)
	{ 
		myAnimationHelper.Play(PlayerAnimations::eMeleeAttack);
	}
	if (myAnimationHelper.GetCurrentMap() == PlayerAnimations::eMovement)
	{
		ModelComponent* mC = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<ModelComponent>(myHostId);
		int partialJoint = Context::Get()->myModelManager->GetBoneNameToIdMap(mC->myModelId)["Spine1_SK"];
		float myInfluence = 0.75f;
		myAnimationHelper.PlayPartially(myAnimationHelper.GetCurrentMap(), PlayerAnimations::eMeleeAttack, partialJoint, myInfluence);
	}

	// Create a new entity
	ISTE::Scene* scene = &myCtx->mySceneHandler->GetActiveScene();
	TransformComponent* transform = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myHostId);
	myAttacksIds["primary"] = scene->NewEntity();

	// Assign all components
	SphereTriggerComponent* trigger = scene->AssignComponent<SphereTriggerComponent>(myAttacksIds["primary"]);
	trigger->myOffset.y += 0.9f;
	TransformComponent* attackTransfrom = scene->AssignComponent<TransformComponent>(myAttacksIds["primary"]);
	attackTransfrom->myPosition = (transform->myPosition - transform->myCachedTransform.GetForwardV3() * (myPrimaryAttackRange));
	AttackValueComponent* attack = scene->AssignComponent<AttackValueComponent>(myAttacksIds["primary"]);

	// Set all values
	attack->myAttackValue = myPrimaryDamage;
	attack->myTag = "PlayerMeleeAttack";
	attack->myExtraInfo = "PrimaryAttack";

	//attackTransfrom->myPosition = { transform->myPosition.x, transform->myPosition.y, transform->myPosition.z };
	attackTransfrom->myScale = { myPrimaryAttackRange, myPrimaryAttackRange, myPrimaryAttackRange};

	// Attack Speed Timer
	myCanUsePrimary = false;
	CountDown attackSpeedTimer;
	attackSpeedTimer.name = "PrimaryAttackSpeedTimer";
	attackSpeedTimer.duration = myPrimaryAttackSpeed;
	attackSpeedTimer.callback = [this]() { myCanUsePrimary = true; };
	myCtx->myTimeHandler->AddTimer(attackSpeedTimer);

	// Stay Trigger Timer
	UpdateTimer attackStayTimer;
	attackSpeedTimer.name = "PrimaryAttackStayTimer" + std::to_string(myAttacksIds["primary"]);
	attackStayTimer.framesToSkip = 1;
	attackSpeedTimer.duration = 0.f;
	attackSpeedTimer.callback = [this, name = attackSpeedTimer.name ]() {
		myCtx->mySceneHandler->GetActiveScene().DestroyEntity(myAttacksIds["primary"]);
		myCtx->myTimeHandler->RemoveTimer(name);
	};
	myCtx->myTimeHandler->AddTimer(attackSpeedTimer);


	// Spawn VFX
	myCtx->myVFXHandler->DeactivateVFX(myPrimaryAttackVFX); // deactivate if one already exist
	myPrimaryAttackVFX = myCtx->myVFXHandler->SpawnVFX("Player_Primary", transform->myPosition, transform->myEuler.GetAngles());
}

void ISTE::PlayerBehaviour::SecondaryAttack()
{

	if (myMana <= mySecondaryAttackCost)
		return;

	// send event
	Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::PlayerUsedAbility, 1);

	myIsStaled = true;
	myStalTimer = 0;
	Reset();

	if(myAnimationHelper.GetCurrentMap() != PlayerAnimations::eThrowAbility)
	{
		myAnimationHelper.ForcePlay(PlayerAnimations::eThrowAbility);
	}
	//else if (myAnimationHelper.GetCurrentMap() == PlayerAnimations::eMovement)
	//{
	//	ModelComponent* mC = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<ModelComponent>(myHostId);
	//	int partialJoint = Context::Get()->myModelManager->GetBoneNameToIdMap(mC->myModelId)["Spine1_SK"];
	//	float myInfluence = 0.95;
	//	myAnimationHelper.PlayPartially(myAnimationHelper.GetCurrentMap(), PlayerAnimations::eThrowAbility, partialJoint, myInfluence);
	//}

	//mySource->SetActiveClip((int)PlayerSounds::eSecondaryAttack);
	//mySource->Play();

	//should not use GPU pick
	//ScreenHelper::GPUPickedData result = myScreenHelper.Pick(myScreenHelper.GetMouseRelativePosition());
	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
	TransformComponent* transform = scene.GetComponent<TransformComponent>(myHostId);

	ScreenHelper::Ray ray = myScreenHelper.GetRayFromPoint(myScreenHelper.GetMouseRelativePosition(), myCtx->myWindow->GetResolution().myValue);
	CU::Vec3f destination;

	const bool destinationFoundInNavMesh = NavMesh::RayIntersectsNavMesh(ray.myOrigin, ray.myDir, destination);

	if (!destinationFoundInNavMesh)
		if (!NavMesh::RayClosestPointIntersection(ray.myOrigin, ray.myDir, 0, transform->myPosition, destination, 20.f))
			return;

	myProjectileCount += 1;


	EntityID entityID = scene.NewEntity();

	std::string id = "projectile" + std::to_string(entityID);
	// Create a new entity
	myProjectileIds[id] = entityID;


	LookAt(*transform, destination);


		CU::Vec3f unitVector = destination - transform->myPosition;
		unitVector.y = 0;

		EntityID ide = Context::Get()->mySystemManager->GetSystem<EnemyBehaviourSystem>()->GetEntityIDFromScreen();
		if (scene.GetComponent<EnemyStatsComponent>(ide))
		{
			TransformComponent* enemyTransform = scene.GetComponent<TransformComponent>(ide);
			TriggerComponent* enemyTrigger = scene.GetComponent<TriggerComponent>(ide);
			SphereTriggerComponent* sphereEnemyTrigger = scene.GetComponent<SphereTriggerComponent>(ide);

			CU::Vec3f enemyOffset;

			if (enemyTrigger != nullptr)
			{
				enemyOffset = enemyTrigger->myOffset;
			}
			else if (sphereEnemyTrigger != nullptr)
			{
				enemyOffset = sphereEnemyTrigger->myOffset;
			}

			unitVector = (enemyTransform->myPosition + enemyOffset) - (transform->myPosition + CU::Vec3f(0, 0.9f, 0));

			if (!CanAttack(transform->myPosition + CU::Vec3f(0, 0.9f, 0), enemyTransform->myPosition + enemyOffset))
				unitVector.y = 0;
		}

		unitVector.Normalize();

	//Assign all components
	scene.AssignComponent<SphereTriggerComponent>(myProjectileIds[id]);
	TransformComponent* attackTransfrom = scene.AssignComponent<TransformComponent>(myProjectileIds[id]);
	AttackValueComponent* attack = scene.AssignComponent<AttackValueComponent>(myProjectileIds[id]);


	attack->myAttackValue = 10;
	attack->myTag = "PlayerRangedAttack";
	attack->myExtraInfo = "SecondaryAttack";
	attack->myIdentifier = id;

	attackTransfrom->myPosition = { transform->myPosition.x, transform->myPosition.y + 0.9f, transform->myPosition.z };
	attackTransfrom->myScale = { 0.2f, 0.2f, 0.2f };

	const int vfxId = myCtx->myVFXHandler->SpawnVFX("Player_Secondary", attackTransfrom->myPosition);
	//CU::Vec2f unitVector = MovementMath::GetUnitVector(transform->myPosition, result.myPosition);

	// Stay Trigger Timer
	CountDown attackStayTimer;
	attackStayTimer.name = "PlayerProjectileAttackStayTimer" + id;
	attackStayTimer.duration = mySecondaryAttackStay;
	attackStayTimer.callback = [this, id, vfxId, attackTransfrom]() {
		//Context::Get()->myVFXHandler->Stop(vfxId);
		Context::Get()->mySceneHandler->GetActiveScene().DestroyEntity(myProjectileIds[id]);
		Context::Get()->myVFXHandler->SpawnVFX("Player_Projectile_Impact", attackTransfrom->myPosition);
		Context::Get()->myVFXHandler->DeactivateVFX(vfxId);
	};
	Context::Get()->myTimeHandler->AddTimer(attackStayTimer);

	// Movement On Projectile For Boris
	UpdateTimer attackMovementTimer;
	attackMovementTimer.name = "PlayerProjectileTimer" + id;
	attackMovementTimer.duration = mySecondaryAttackStay - 0.01f;
	attackMovementTimer.callback = [this, unitVector, attackTransfrom, vfxId]() {
		float deltaTime = Context::Get()->myTimeHandler->GetDeltaTime();
		attackTransfrom->myPosition += unitVector * mySecondaryAttackMovmentSpeed * deltaTime;/*{ attackTransfrom->myPosition.x + unitVector.x * deltaTime * mySecondaryAttackMovmentSpeed,
									attackTransfrom->myPosition.y,
									attackTransfrom->myPosition.z + unitVector.y * deltaTime * mySecondaryAttackMovmentSpeed };*/
		Context::Get()->myVFXHandler->SetVFXPosition(vfxId, attackTransfrom->myPosition);
		Context::Get()->myVFXHandler->SetVFXRotation(vfxId, attackTransfrom->myEuler.GetAngles());
	};
	Context::Get()->myTimeHandler->AddTimer(attackMovementTimer);

	myMana -= mySecondaryAttackCost;
	myCtx->myEventHandler->InvokeEvent(EventType::PlayerSpentMana, INVALID_ENTITY);

	//mySource->SetActiveClip((int)PlayerSounds::eSecondaryAttack);
	//mySource->Play();

	mySource->Play((int)PlayerSounds::eSecondaryAttack, ASP_ADDITIVE);
}

void ISTE::PlayerBehaviour::MagicArmor()
{
	if (!myCanUseArmor) // Not on cooldown
		return;

	if (myMana - myArmorCost <= 0.f) // The cost is affordable
		return;

	Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
	// send event
	Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::PlayerUsedAbility, 2);

	myMagicArmorVFX = myCtx->myVFXHandler->SpawnVFX("Player_Armor");
	
	if (myAnimationHelper.GetCurrentMap() == PlayerAnimations::eIdle)
	{
		myAnimationHelper.Play(PlayerAnimations::eSpell);
	}
	else if (myAnimationHelper.GetLastMap() != PlayerAnimations::eIdle && myAnimationHelper.GetCurrentMap() == PlayerAnimations::eMovement)
	{
		ModelComponent* mC = scene.GetComponent<ModelComponent>(myHostId);
		int partialJoint = Context::Get()->myModelManager->GetBoneNameToIdMap(mC->myModelId)["Spine1_SK"];
		float myInfluence = 0.90;
		myAnimationHelper.PlayPartially(myAnimationHelper.GetCurrentMap(), PlayerAnimations::eSpell, partialJoint, myInfluence);
	}
	//mySource->SetActiveClip((int)PlayerSounds::eMagicArmor);
	//mySource->Play();

	mySource->Play((int)PlayerSounds::eMagicArmor);

	myMana -= myArmorCost;
	myCtx->myEventHandler->InvokeEvent(EventType::PlayerSpentMana, INVALID_ENTITY);
	myArmored = true;
	myCanUseArmor = false;
	{
		CountDown armorCooldown;
		armorCooldown.name = "Player_Cooldown_Armor";
		armorCooldown.duration = myArmorCooldown;
		armorCooldown.callback = [this]() { myCanUseArmor = true; };
		myCtx->myTimeHandler->AddTimer(armorCooldown);

		EntityID armorTriggerId = scene.NewEntity();
		SphereTriggerComponent* armorTrigger = scene.AssignComponent<SphereTriggerComponent>(armorTriggerId);
		TransformComponent* armorTransform = scene.AssignComponent<TransformComponent>(armorTriggerId);
		ProjectileBlockBehaviour* pbb = scene.AssignBehaviour<ProjectileBlockBehaviour>(armorTriggerId);
		ComponentID compId = myCtx->mySceneHandler->GetId<TransformComponent>();

		UpdateTimer armorTriggerTimer;
		armorTriggerTimer.duration = myArmorDuration;
		armorTriggerTimer.callback = [compId, pIndex = GetEntityIndex(myHostId), aIndex = GetEntityIndex(armorTriggerId)]() {
			Scene& scene = Context::Get()->mySceneHandler->GetActiveScene();
			ComponentPool& tPool = scene.GetComponentPool(compId);
			TransformComponent* playerTransform = (TransformComponent*)tPool.Get(pIndex);
			TransformComponent* armorTransform = (TransformComponent*)tPool.Get(aIndex);
			if (playerTransform != nullptr && armorTransform != nullptr)
			{
				armorTransform->myPosition = playerTransform->myPosition;
				armorTransform->myPosition.y += 0.9f;
			}
		};
		myCtx->myTimeHandler->AddTimer(armorTriggerTimer);
	
		CountDown armorDuration;
		armorDuration.name = "armorCooldown";
		armorDuration.duration = myArmorDuration;
		armorDuration.callback = [this, armorTriggerId]() {
			myArmored = false; 
			Context::Get()->myVFXHandler->DeactivateVFX(myMagicArmorVFX);
			Context::Get()->mySceneHandler->GetActiveScene().DestroyEntity(armorTriggerId);
			myMagicArmorVFX = -1;
		};
		myCtx->myTimeHandler->AddTimer(armorDuration);
	}
}

void ISTE::PlayerBehaviour::Teleport()
{
	if (!myCanUseTeleport) // Not on cooldown
		return;

	if (myMana - myTeleportCost <= 0.f) // The cost is affordable
		return;

	NavMesh::NavMesh* navMesh = &Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh();

	if (navMesh == nullptr)
		return;

	TransformComponent* transform = myCtx->Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myHostId);
	const CU::Vec3f& start = transform->myPosition;
	const ScreenHelper::Ray ray = myScreenHelper.GetRayFromPoint(myScreenHelper.GetMouseRelativePosition(), myCtx->myWindow->GetResolution().myValue);
	CU::Vec3f destination;

	const bool destinationFoundInNavMesh = NavMesh::RayIntersectsNavMesh(ray.myOrigin, ray.myDir, destination);

	if (!destinationFoundInNavMesh)
	{
		if (!NavMesh::RayClosestPointIntersection(ray.myOrigin, ray.myDir, 0, start, destination, myMaxTeleportDistance, true))
			return;
	}

	std::queue<CU::Vec3f> garb;

	std::vector<int> val = NavMesh::FindIndexPath(start, destination);

	
	//This is where I end it
	if (val.size() != 0)
	{
		NavMesh::FindPointPath(start, destination, garb, true);

		float totalDist = 0;
		CU::Vec3f lastDestPoint = start;
		CU::Vec3f destPoint;

		bool inDist = true;

		while (!garb.empty())
		{
			destPoint = garb.front();
			garb.pop();

			totalDist += (lastDestPoint - destPoint).Length();
			lastDestPoint = destPoint;

			if (totalDist >= myMaxTeleportDistance)
			{
				inDist = false;
				garb = std::queue<CU::Vec3f>();
				break;
			}
		}

		if (inDist)
		{
			Teleportation(transform, destination);
		}
		else
		{
			//this would allow for long distance teleportation even if navmesh is connected
			if (((destination - start) * CU::Vec3f(1, 8.f, 1)).Length() <= myMaxTeleportDistance)
			{
				Teleportation(transform, destination);
			}
			else
			{
				std::vector<NavMesh::Intersection> inters;
				NavMesh::GetIntersections(*navMesh, start, destination, inters);

				for (auto& tri : inters)
				{
					for (auto& inter : tri.myIntersections)
					{
						//should messure against intersection points
						val = NavMesh::FindIndexPath(start, inter);

						if (val.size() != 0)
						{
							NavMesh::FindPointPath(start, inter, garb, true);

							float totalDist = 0;
							CU::Vec3f lastDestPoint = start;
							CU::Vec3f destPoint;

							bool inDist = true;

							while (!garb.empty())
							{
								destPoint = garb.front();
								garb.pop();

								totalDist += (lastDestPoint - destPoint).Length();
								lastDestPoint = destPoint;

								if (totalDist >= myMaxTeleportDistance)
								{
									inDist = false;
									garb = std::queue<CU::Vec3f>();
									break;
								}
							}

							if (inDist)
							{
								Teleportation(transform, destPoint);
								return;
							}
						}
					}
				}
			}
		}
	}
	else
	{
		if (((destination - start) * CU::Vec3f(1, 8.f, 1)).Length() <= myMaxTeleportDistance)
		{
			Teleportation(transform, destination);
		}
		else
		{
			std::vector<NavMesh::Intersection> inters;
			NavMesh::GetIntersections(*navMesh, start, destination, inters);

			for (auto& tri : inters)
			{
				for (auto& inter : tri.myIntersections)
				{
					//should messure against intersection points
					val = NavMesh::FindIndexPath(start, inter);

					if (val.size() != 0)
					{
						NavMesh::FindPointPath(start, inter, garb, true);

						float totalDist = 0;
						CU::Vec3f lastDestPoint = start;
						CU::Vec3f destPoint;

						bool inDist = true;

						while (!garb.empty())
						{
							destPoint = garb.front();
							garb.pop();

							totalDist += (lastDestPoint - destPoint).Length();
							lastDestPoint = destPoint;

							if (totalDist >= myMaxTeleportDistance)
							{
								inDist = false;
								garb = std::queue<CU::Vec3f>();
								break;
							}
						}

						if (inDist)
						{
							Teleportation(transform, destPoint);
							return;
						}
					}
				}
			}
		}
	}
}

void ISTE::PlayerBehaviour::Teleportation(TransformComponent* aTransform, CU::Vec3f aDestination)
{
	// send event
	Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::PlayerUsedAbility, 4);
	Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::PlayerTeleportedDistance, (aTransform->myPosition - aDestination).Length() * 100.f);

	myMana -= myTeleportCost;
	myCtx->myEventHandler->InvokeEvent(EventType::PlayerSpentMana, INVALID_ENTITY);
	//transform->myPosition = destination;
	myCtx->myVFXHandler->SpawnVFX("Player_Teleport_Origin", aTransform->myPosition);
	aTransform->myPosition = aDestination;

	//mySource->SetActiveClip((int)PlayerSounds::eTeleport);
	//mySource->Play();
	
	//shouldnt this have an animation?
	//if (myAnimationHelper.GetCurrentMap() == PlayerAnimations::eIdle)
	//{
	//	myAnimationHelper.Play(PlayerAnimations::eSpell);
	//}
	//else if (myAnimationHelper.GetLastMap() != PlayerAnimations::eIdle && myAnimationHelper.GetCurrentMap() == PlayerAnimations::eMovement)
	//{
	//	ModelComponent* mC = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<ModelComponent>(myHostId);
	//	int partialJoint = Context::Get()->myModelManager->GetBoneNameToIdMap(mC->myModelId)["Spine1_SK"];
	//	float myInfluence = 0.80;
	//	myAnimationHelper.PlayPartially(myAnimationHelper.GetCurrentMap(), PlayerAnimations::eSpell, partialJoint, myInfluence);
	//}

	mySource->Play((int)PlayerSounds::eTeleport);
	myCtx->myVFXHandler->SpawnVFX("Player_Teleport", aTransform->myPosition);
	Reset();

	// Teleport Cooldown Timer
	myCanUseTeleport = false;
	CountDown teleportCooldown;
	teleportCooldown.name = "Player_Cooldown_Teleport";
	teleportCooldown.duration = myTeleportCooldown;
	teleportCooldown.callback = [this]() { myCanUseTeleport = true; };
	myCtx->myTimeHandler->AddTimer(teleportCooldown);
}

void ISTE::PlayerBehaviour::AoEDoT()
{
	if (!myCanUseAoEDoT) // Not on cooldown
		return;

	if (myMana - myAoEDoTCost <= 0.f) // The cost is affordable
		return;

	// send event
	Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::PlayerUsedAbility, 3);
	
	if (myAnimationHelper.GetCurrentMap() == PlayerAnimations::eIdle)
	{
		myAnimationHelper.Play(PlayerAnimations::eSpell);
	}
	else if (myAnimationHelper.GetLastMap() != PlayerAnimations::eIdle && myAnimationHelper.GetCurrentMap() == PlayerAnimations::eMovement)
	{
		ModelComponent* mC = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<ModelComponent>(myHostId);
		int partialJoint = Context::Get()->myModelManager->GetBoneNameToIdMap(mC->myModelId)["Spine1_SK"];
		float myInfluence = 0.95;
		myAnimationHelper.PlayPartially(myAnimationHelper.GetCurrentMap(), PlayerAnimations::eSpell, partialJoint, myInfluence);
	}
	//mySource->SetActiveClip((int)PlayerSounds::eAOE);
	//mySource->Play();
	mySource->Play((int)PlayerSounds::eAOE);

	myMana -= myAoEDoTCost;
	myCtx->myEventHandler->InvokeEvent(EventType::PlayerSpentMana, INVALID_ENTITY);
	// Create a new entity
	ISTE::Scene* scene = &myCtx->mySceneHandler->GetActiveScene();
	TransformComponent* transform = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myHostId);
	myAttacksIds["AoEDoT"] = scene->NewEntity();

	// Assign all components
	scene->AssignComponent<SphereTriggerComponent>(myAttacksIds["AoEDoT"]);
	TransformComponent* attackTransfrom = scene->AssignComponent<TransformComponent>(myAttacksIds["AoEDoT"]);
	AttackValueComponent* attack = scene->AssignComponent<AttackValueComponent>(myAttacksIds["AoEDoT"]);

	// Set all values
	attack->myAttackValue = myAoEDoTDamage;
	attack->myTag = "PlayerMeleeAttack";
	attack->myExtraInfo = "DamageOverTime";

	attackTransfrom->myPosition = { transform->myPosition.x, transform->myPosition.y, transform->myPosition.z };
	attackTransfrom->myScale = { myAoEDoTRadius, myAoEDoTRadius, myAoEDoTRadius };

	myAreaOfEffectVFX = myCtx->myVFXHandler->SpawnVFX("Player_AoEDoT", attackTransfrom->myPosition);

	// Attack Speed Timer
	{
		myCanUseAoEDoT = false;
		CountDown aoeDoTSpeedTimer;
		aoeDoTSpeedTimer.name = "Player_Cooldown_AoEDoT";
		aoeDoTSpeedTimer.duration = myAoEDoTCooldown;
		aoeDoTSpeedTimer.callback = [this]() { myCanUseAoEDoT = true; };
		myCtx->myTimeHandler->AddTimer(aoeDoTSpeedTimer);
	}

	// Stay Trigger Timer
	{
		myDoTUsed = true;
		CountDown aoeDoTStayTimer;
		aoeDoTStayTimer.name = "AoEDoTStayTimer";
		aoeDoTStayTimer.duration = myAoEDoTDuration;
		aoeDoTStayTimer.callback = [ this ]() {
			myDoTUsed = false;
			myCtx->mySceneHandler->GetActiveScene().DestroyEntity(myAttacksIds["AoEDoT"]);
			Context::Get()->myVFXHandler->KillVFX(myAreaOfEffectVFX);
			myAreaOfEffectVFX = -1;
		};
		myCtx->myTimeHandler->AddTimer(aoeDoTStayTimer);
	}
}

void ISTE::PlayerBehaviour::Ultimate()
{
	if (!myCanUseUltimate) // Not on cooldown
		return;

	if (myMana - myUltimateCost <= 0.f) // The cost is affordable
		return;

	NavMesh::NavMesh* navMesh = &Context::Get()->mySceneHandler->GetActiveScene().GetNavMesh();

	if (navMesh == nullptr)
		return;

	TransformComponent* transform = myCtx->Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(myHostId);
	const CU::Vec3f& start = transform->myPosition;
	const ScreenHelper::Ray ray = myScreenHelper.GetRayFromPoint(myScreenHelper.GetMouseRelativePosition(), myCtx->myWindow->GetResolution().myValue);
	CU::Vec3f destination;

	const bool destinationFoundInNavMesh = NavMesh::RayIntersectsNavMesh(ray.myOrigin, ray.myDir, destination);

	if (!destinationFoundInNavMesh || (destination - transform->myPosition).Length() > myUltimateRange)
		return;

	// send event
	Context::Get()->myEventHandler->InvokeEvent(ISTE::EventType::PlayerUsedAbility, 5);

	
	if (myAnimationHelper.GetCurrentMap() == PlayerAnimations::eIdle)
	{
		myAnimationHelper.Play(PlayerAnimations::eSpell);
	}
	else if (myAnimationHelper.GetLastMap() != PlayerAnimations::eIdle && myAnimationHelper.GetCurrentMap() == PlayerAnimations::eMovement)
	{
		ModelComponent* mC = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<ModelComponent>(myHostId);
		int partialJoint = Context::Get()->myModelManager->GetBoneNameToIdMap(mC->myModelId)["Spine1_SK"];
		float myInfluence = 1.f;
		myAnimationHelper.PlayPartially(myAnimationHelper.GetCurrentMap(), PlayerAnimations::eSpell, partialJoint, myInfluence);
	}

	mySource->Play((int)PlayerSounds::eAOESIMExplode);

	myMana -= myUltimateCost;
	myCtx->myEventHandler->InvokeEvent(EventType::PlayerSpentMana, INVALID_ENTITY);
	myHealth += myUltimateHealthRegen;

	if (myHealth > myHealthMax)
	{
		myHealth = myHealthMax;
	}

	Reset();


	// Create a new entity
	ISTE::Scene* scene = &myCtx->mySceneHandler->GetActiveScene();
	myAttacksIds["Ultimate"] = scene->NewEntity();

	// Assign all components
	scene->AssignComponent<SphereTriggerComponent>(myAttacksIds["Ultimate"]);
	TransformComponent* attackTransfrom = scene->AssignComponent<TransformComponent>(myAttacksIds["Ultimate"]);

	attackTransfrom->myPosition = destination;
	attackTransfrom->myScale = { myUltimateRadius, myUltimateRadius, myUltimateRadius };


	myCtx->myModelVFXHandler->Spawn("Player_Ultimate_Axe", attackTransfrom->myPosition);
	CountDown timer;
	timer.callback = [pos = attackTransfrom->myPosition, this ]() {
		Context::Get()->myVFXHandler->SpawnVFX("Player_Ultimate", pos);
		// Stay Trigger Timer

		AttackValueComponent* attack = Context::Get()->mySceneHandler->GetActiveScene().AssignComponent<AttackValueComponent>(myAttacksIds["Ultimate"]);

		// Set all values
		attack->myAttackValue = myUltimateDamage;
		attack->myTag = "PlayerRangedAttack";
		attack->myExtraInfo = "DamageOverTime";

		CountDown ultimateStayTimer;
		ultimateStayTimer.name = "UltimateAttackStayTimer";
		ultimateStayTimer.duration = 1.5f; // I assume attacks match invincibility time to only occur once.
		ultimateStayTimer.callback = [attackId = myAttacksIds["Ultimate"]]() {
			Context::Get()->mySceneHandler->GetActiveScene().DestroyEntity(attackId);
			//Context::Get()->myVFXHandler->DeactivateVFX(vfx);
		};
		Context::Get()->myTimeHandler->AddTimer(ultimateStayTimer);
	};
	timer.duration = 0.7f;
	myCtx->myTimeHandler->AddTimer(timer);

	//const int vfxId = myCtx->myVFXHandler->SpawnVFX("Player_Ultimate", attackTransfrom->myPosition);
	// Attack Speed Timer
	{
		myCanUseUltimate = false;
		CountDown ultimateSpeedTimer;
		ultimateSpeedTimer.name = "Player_Cooldown_Ultimate";
		ultimateSpeedTimer.duration = myUltimateCooldown;
		ultimateSpeedTimer.callback = [this]() { myCanUseUltimate = true; };
		myCtx->myTimeHandler->AddTimer(ultimateSpeedTimer);
	}

	//myCtx->myVFXHandler->Play(myBeamVFX);
}

void ISTE::PlayerBehaviour::InitMoveDistanceTimer()
{
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	TransformComponent* t = scene.GetComponent<TransformComponent>(myHostId);
	ComponentID compId = myCtx->mySceneHandler->GetId<TransformComponent>();
	myLastMovePosition = t->myPosition;

	myMoveDistanceTimer = new LocalStepTimer();
	myMoveDistanceTimer->SetDelay(0.5f);
	myMoveDistanceTimer->SetCallback([this, compId, index = GetEntityIndex(myHostId)]() {
		ComponentPool& compPool = myCtx->mySceneHandler->GetActiveScene().GetComponentPool<TransformComponent>();
		TransformComponent* t = (TransformComponent*)compPool.Get(index);
		Context::Get()->myEventHandler->InvokeEvent(EventType::PlayerMoveDistance, (t->myPosition - myLastMovePosition).Length() * 100.f);
		myLastMovePosition = t->myPosition;
	});
}

void ISTE::PlayerBehaviour::UpdateWithDatabase(CU::Database<true>& aBehaviourDatabase)
{
	InitAbilities(); //I've done so much copy and paste please just let me take this shortcut.
	InitAnimationLerpSpeeds(); //I've done so much copy and paste please just let me take this shortcut.
}
