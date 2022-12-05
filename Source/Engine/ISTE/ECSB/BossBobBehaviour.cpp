#include "BossBobBehaviour.h"
#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/StateStack/StateManager.h"
#include "ISTE/StateStack/StateType.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/ModelComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimatorComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/CustomShaderComponent.h"
#include "ISTE/Graphics/Resources/ModelManager.h"
#include "ISTE/Graphics/Resources/AnimationManager.h"
#include "ISTE/ComponentsAndSystems/EnemyStatsComponent.h"
#include "ISTE/ComponentsAndSystems/AttackValueComponent.h"
#include "ISTE/Navigation/PathFinding.h"
#include "ISTE/Physics/ComponentsAndSystems/TriggerComponent.h"
#include "ISTE/Physics/ComponentsAndSystems/SphereTriggerComponent.h"
#include "ISTE/ComponentsAndSystems/EnemyStatsComponent.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Time/TimerDefines.h"
#include "ISTE/Audio/AudioSource.h"
#include "ISTE/CU/Database.h"
#include "ISTE/VFX/VFXHandler.h"
#include "ISTE/VFX/SpriteParticles/Sprite2DParticleHandler.h"
#include "ISTE/Events/EventHandler.h"
#include "ISTE/ComponentsAndSystems/EnemyBehaviourSystem.h"

#include <fstream>
#include "ISTE/CU/UtilityFunctions.hpp"

#include "ISTE/CU/MemTrack.hpp"

#include "ISTE/Graphics/ComponentAndSystem/MaterialComponent.h"
#include "ISTE/Graphics/ComponentAndSystem/DecalComponent.h"
#include "ISTE/Graphics/Resources/TextureManager.h"

// Other Behaviours
#include "ISTE/ECSB/IdleEnemyBehaviour.h"
#include "ISTE/ECSB/LurkerEnemyBehaviour.h"
#include "ISTE/ECSB/HunterEnemyBehaviour.h"

// Player Behaviour
#include "PlayerBehaviour.h"


#ifndef NDEBUG
#define assert(Expr, Msg) \
{ \
	if (!Expr) \
	{ \
		std::cerr << "Assert Failed: " << Msg << "\n" \
				<< "Expected: " << #Expr << "\n" \
				<< "Source: " << __FILE__ << ", line " << __LINE__ << std::endl; \
		abort(); \
	} \
} 
#else
#define assert(Expr, Msg);
#endif


void ISTE::BossBobBehaviour::Init()
{
	myActiveScene = &ISTE::Context::Get()->mySceneHandler->GetActiveScene();
	myStats = myActiveScene->GetComponent<EnemyStatsComponent>(myHostId);
	myTransform = myActiveScene->GetComponent<ISTE::TransformComponent>(myHostId);
	myPlayerTransform = myActiveScene->GetComponent<TransformComponent>(myActiveScene->GetPlayerId());

	LoadStageData();

	myStage = 0;
	myAttackInt = 0;

	CheckStage();

	myIsActive = false;
	myIsIdle = true;
	myOnIFrameMode = false;
	myUnderOrderDelay = false;
	myDoingAnAttack = false;

	myPlayerLastSeenPos = myTransform->myPosition; // Default;

	myIdleAnim = Context::Get()->myAnimationManager->LoadAnimation(myHostId, "../Assets/Animations/CH_NPC_Bob_idle_ANIM.fbx");
	myDeathAnim = Context::Get()->myAnimationManager->LoadAnimation(myHostId, "../Assets/Animations/CH_NPC_Bob_Death_ANIM.fbx");
	myRoarAnim = Context::Get()->myAnimationManager->LoadAnimation(myHostId, "../Assets/Animations/CH_NPC_Bob_Roar_ANIM.fbx");
	myAcidSpitAnim = Context::Get()->myAnimationManager->LoadAnimation(myHostId, "../Assets/Animations/CH_NPC_Bob_AcidSpit_ANIM.fbx");
	myStabSweepAnim = Context::Get()->myAnimationManager->LoadAnimation(myHostId, "../Assets/Animations/CH_NPC_Bob_StabSweep_ANIM.fbx");
	myStabsAnim = Context::Get()->myAnimationManager->LoadAnimation(myHostId, "../Assets/Animations/CH_NPC_Bob_Stage3_ANIM.fbx");
	myStuckAnim = Context::Get()->myAnimationManager->LoadAnimation(myHostId, "../Assets/Animations/CH_NPC_Bob_Stuck_ANIM.fbx");
	myHeadStuckReverseAnim = Context::Get()->myAnimationManager->LoadAnimation(myHostId, "../Assets/Animations/CH_NPC_Bob_HeadStuckReverse_ANIM.fbx");
	myHeadbangAnim = Context::Get()->myAnimationManager->LoadAnimation(myHostId, "../Assets/Animations/CH_NPC_Bob_Headbang_ANIM.fbx");
	mySadAnim = Context::Get()->myAnimationManager->LoadAnimation(myHostId, "../Assets/Animations/CH_NPC_Bob_Sad_ANIM.fbx");

	myAnimationHelper.SetEntityID(myHostId);
	myAnimationHelper.MapAnimation(BossAnimations::eIdle, myIdleAnim, AH_LOOPING, 0, 0);
	myAnimationHelper.MapAnimation(BossAnimations::eDeath, myDeathAnim, 0, 0, 0);
	myAnimationHelper.MapAnimation(BossAnimations::eRoar, myRoarAnim, 0, 0, 0);
	myAnimationHelper.MapAnimation(BossAnimations::eAcidSpit, myAcidSpitAnim, 0, 0, 0);
	myAnimationHelper.MapAnimation(BossAnimations::eStabSweep, myStabSweepAnim, 0, 0, 0);
	myAnimationHelper.MapAnimation(BossAnimations::eStabs, myStabsAnim, 0, 0, 0);
	myAnimationHelper.MapAnimation(BossAnimations::eHeadbang, myHeadbangAnim, 0, 0, 0);
	myAnimationHelper.MapAnimation(BossAnimations::eHeadStuckReverse, myHeadStuckReverseAnim, 0, 0, 0);
	myAnimationHelper.MapAnimation(BossAnimations::eStuck, myStuckAnim, AH_LOOPING, 0, 0);
	myAnimationHelper.MapAnimation(BossAnimations::eSad, mySadAnim, 0, 0, 0);

	myAnimationHelper.Play(BossAnimations::eIdle);

	AudioSource* audio = myActiveScene->GetComponent<AudioSource>(myHostId);

	audio->SetSoundType(ISTE::SoundTypes::eSFX);

	audio->LoadAndMapClip(0, "../Assets/Audio/Sounds/Bob_IntroRoar.wav", false);
	audio->LoadAndMapClip(1, "../Assets/Audio/Sounds/Bob_CallForHelp.wav", false);
	audio->LoadAndMapClip(2, "../Assets/Audio/Sounds/Bob_HeadDash.wav", false);
	audio->LoadAndMapClip(3, "../Assets/Audio/Sounds/Bob_ReleaseHead.wav", false);
	audio->LoadAndMapClip(4, "../Assets/Audio/Sounds/Bob_SweepAttack.wav", true);
	audio->LoadAndMapClip(5, "../Assets/Audio/Sounds/Bob_SweepRoar.wav", false);
	audio->LoadAndMapClip(6, "../Assets/Audio/Sounds/Bob_AcidAttack.wav", false);
	audio->LoadAndMapClip(7, "../Assets/Audio/Sounds/Bob_StabAttack.wav", false);
}

void ISTE::BossBobBehaviour::Update(float aDeltaTime)
{
	if (myStats->myIsDead)
		return;

	if (!myIsActive)
	{
		if (!myAnimationHelper.IsPlaying())
			myAnimationHelper.Play(BossAnimations::eIdle);
		return;
	}

	if (myOnIFrameMode)
		OnIFrame();

	if (!myDoingAnAttack)
		CheckStage(); // To check health percentage.

	if (myIsIdle && !myUnderOrderDelay) // Start order delay.
		StartIdleTimer();

	if (!myIsIdle && !myUnderOrderDelay && !myDoingAnAttack) // Get a random state order.
		GetNewStateOrder();

	if (!myIsIdle && !myUnderOrderDelay && myDoingAnAttack) // Do state order.
		DoStateOrder(aDeltaTime);

	if (!myAnimationHelper.IsPlaying())
		myAnimationHelper.Play(BossAnimations::eIdle);

	if (myUnderOrderDelay)
		Rotate(myActiveScene->GetComponent<TransformComponent>(myActiveScene->GetPlayerId())->myPosition, myRotationSpeed * aDeltaTime);
}

void ISTE::BossBobBehaviour::OnTrigger(EntityID aId)
{
	if (myStats->myIsDead)
		return;

	if (!myIsActive)
		return;

	AttackValueComponent* attacked = myActiveScene->GetComponent<AttackValueComponent>(aId);
	if (attacked == nullptr) // Haha, not attacked this time.
		return;

	if (attacked->myTag != "PlayerMeleeAttack" && attacked->myTag != "PlayerRangedAttack")
		return;

	if (attacked->myTag == "PlayerMeleeAttack" && !myStabAndStuckData[myStage].IsStuck)
		return;

	if (attacked->myExtraInfo == "DamageOverTime")
	{
		const float dt = Context::Get()->myTimeHandler->GetDeltaTime();
		myStats->myHealth -= attacked->myAttackValue * ((myStabAndStuckData[myStage].IsStuck) ? myStabAndStuckData[myStage].ExtraDamage : 1.f) * dt;
	}
	else
		myStats->myHealth -= attacked->myAttackValue * ((myStabAndStuckData[myStage].IsStuck) ? myStabAndStuckData[myStage].ExtraDamage : 1.f);

	Context::Get()->myEventHandler->InvokeEvent(EventType::BossTookDamage, INVALID_ENTITY);

	if (attacked != nullptr && attacked->myExtraInfo == "SecondaryAttack")
	{
		Context::Get()->myTimeHandler->InvokeTimer("PlayerProjectileAttackStayTimer" + attacked->myIdentifier);

		Context::Get()->myTimeHandler->RemoveTimer("PlayerProjectileAttackStayTimer" + attacked->myIdentifier);
		Context::Get()->myTimeHandler->RemoveTimer("PlayerProjectileTimer" + attacked->myIdentifier);

		//if we wanna do single hits in a very ugly way we could
		attacked->myExtraInfo = "";
	}

	if (myOnIFrameMode)
		return;



	// I Frame Timer
	myOnIFrameMode = true;
	CountDown iFrameTimer;
	iFrameTimer.name = "EnemyIFrameTimer";
	iFrameTimer.duration = myDamageShaderDuration;
	iFrameTimer.callback = [this]() { myOnIFrameMode = false; };
	Context::Get()->myTimeHandler->AddTimer(iFrameTimer);



	DamageShader();

	if (myStats->myHealth <= 0.f)
	{
		myStats->myIsDead = true;

		myAnimationHelper.Play(BossAnimations::eDeath);

		// Spawn VFX
		CU::Vec3f offset(0.f, -2.5f, 0.f);
		std::cout << myTransform->myPosition << std::endl;
		Context::Get()->myVFXHandler->SpawnVFX("Bob_Dying", myTransform->myPosition + offset);
		Context::Get()->myVFXHandler->SpawnVFX("Bob_LyingDead", myTransform->myPosition);

		// Kill all enemies
		EnemyBehaviourSystem* enemySystem = Context::Get()->mySystemManager->GetSystem<EnemyBehaviourSystem>();
		enemySystem->KillAll();

		Context::Get()->myEventHandler->InvokeEvent(EventType::BobDied, 0);

		CountDown bossDeathTimer;
		bossDeathTimer.name = "Win Timer";
		bossDeathTimer.duration = 10.f; // time it takes for Bob to play death anim or something
		bossDeathTimer.callback = [this]() {
			Context::Get()->myGenericDatabase->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "Win Screen");
			Context::Get()->myStateManager->PushState(ISTE::StateType::FadeOut);
		};
		Context::Get()->myTimeHandler->AddTimer(bossDeathTimer);
	}
}

const float ISTE::BossBobBehaviour::GetHealth()
{
	return myStats->myHealth;
}

void ISTE::BossBobBehaviour::CheckStage()
{
	float result = myStats->myHealth / myStats->myMaxHealth;

	if (result > 0.75f) // Uncomment this when all type of attacks are done.
		if (myStage == 0) // No needed for switching
			return;
		else
			SwitchStage(0); // This is for data's sake.
	else if (result > 0.5f)
		if (myStage == 1)
			return;
		else
			SwitchStage(1);
	else if (result > 0.25f)
		if (myStage == 2)
			return;
		else
			SwitchStage(2);
	else
		if (myStage == 3)
			return;
		else
			SwitchStage(3);
}

void ISTE::BossBobBehaviour::SwitchStage(int aNewStage)
{
	if (myDoingAnAttack)
	{
		switch (myState) // Resetting all structs in stage.
		{
		case ISTE::BossState::eNotInFight: // Why are you here? Should not be in this one. Lol
			break;
		case ISTE::BossState::eAcid:
		{
			ResetAcidSpit();
			break;
		}
		case ISTE::BossState::eStabSweep:
		{
			ResetStabSweep();
			break;
		}
		case ISTE::BossState::eStabAndStuck:
		{
			ResetStabAndStuck();
			break;
		}
		case ISTE::BossState::ePlzHelp:
		{
			ResetRoarHelp();
			break;
		}
		default: // Something is wrong.
			break;
		}
	}

	myAnimationHelper.Play(BossAnimations::eSad);

	myDoingAnAttack = false;
	myIsIdle = true;
	myUnderOrderDelay = false;
	myAttackInt = 0;
	myStage = aNewStage;
}

void ISTE::BossBobBehaviour::LoadStageData()
{
	UpdateWithDatabase(Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourDatabase());
}
void ISTE::BossBobBehaviour::UpdateWithDatabase(CU::Database<true>& aBehaviourDatabase)
{
	CU::Database<true>& behaviours = Context::Get()->mySceneHandler->GetActiveScene().GetBehaviourDatabase();
	std::vector<int>* attackOptions;
	myStats->myHealth = behaviours.Get<float>("BossHealth");
	myDamageShaderDuration = behaviours.Get<float>("BossDamageDuration");
	myStats->myName = behaviours.Get<std::string>("BossName");
	myRotationSpeed = behaviours.Get<float>("BossRotationSpeed");
	myStats->myMaxHealth = myStats->myHealth;
	AcidSpit tryAcid;

	myAttackOptions.clear();
	myOrderDelays.clear();
	myAcidSpitData.clear();
	myStabSweepData.clear();
	myStabAndStuckData.clear();
	myRoarHelpData.clear();

	for (int i = 0; true; i++)
	{
		if (!behaviours.TryGet<std::vector<int>*>("AttackOptions" + std::to_string(i), attackOptions))
		{
			break;
		}
		myAttackOptions.push_back(*attackOptions);
		myOrderDelays.push_back(behaviours.Get<float>("orderdelay" + std::to_string(i)));
		myAcidSpitData.push_back(behaviours.Get<AcidSpit>("AcidSpit" + std::to_string(i)));
		myStabSweepData.push_back(behaviours.Get<StabSweep>("StabSweep" + std::to_string(i)));
		myStabAndStuckData.push_back(behaviours.Get<StabAndStuck>("StabAndStuck" + std::to_string(i)));
		myRoarHelpData.push_back(behaviours.Get<RoarHelp>("RoarHelp" + std::to_string(i)));
	}
}

void ISTE::BossBobBehaviour::SetAllSpawnPools(std::vector<CU::Vec3f> aPool)
{
	for (size_t i = 0; i < myRoarHelpData.size(); i++)
	{
		myRoarHelpData[i].SpawnPool = aPool;
		myRoarHelpData[i].SpawnPoolSize = aPool.size();
	}
}

void ISTE::BossBobBehaviour::SetSpawnPool(std::vector<CU::Vec3f> aPool, int aStage)
{
	myRoarHelpData[aStage].SpawnPool = aPool;
	myRoarHelpData[aStage].SpawnPoolSize = aPool.size();
}

void ISTE::BossBobBehaviour::Rotate(CU::Vec3f aPos, float aLerpValue)
{
	//lookat -- made by Lucas (:
	CU::Vec3f tmp = MovementMath::GetUnitVector3(aPos, myTransform->myPosition);
	tmp = CU::Vec3f::Lerp(myTransform->myCachedTransform.GetForwardV3(), tmp, aLerpValue);
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
}

void ISTE::BossBobBehaviour::DamageShader()
{
	ModelComponent* mC = myActiveScene->GetComponent<ModelComponent>(myHostId);
	float totalTimeSin = sin(Context::Get()->myTimeHandler->GetTotalTime() * 20) * 0.5 + 0.5;
	mC->myColor = { totalTimeSin,0,0 };

	// Shader Timer
	CountDown shaderTimer;
	shaderTimer.name = "BossShaderTimer";
	shaderTimer.duration = myDamageShaderDuration;
	shaderTimer.callback = [this]() { myActiveScene->GetComponent<ModelComponent>(myHostId)->myColor = { 1,1,1 }; };
	Context::Get()->myTimeHandler->AddTimer(shaderTimer);
}

void ISTE::BossBobBehaviour::OnIFrame()
{
	ModelComponent* mC = myActiveScene->GetComponent<ModelComponent>(myHostId);
	float totalTimeSin = sin(Context::Get()->myTimeHandler->GetTotalTime() * 20) * 0.5 + 0.5;
	mC->myColor = { totalTimeSin,0,0 };
}

void ISTE::BossBobBehaviour::StartIdleTimer()
{
	myUnderOrderDelay = true;

	if (!myAnimationHelper.IsPlaying())
		myAnimationHelper.Play(BossAnimations::eIdle);

	CountDown bossIdleTimer;
	bossIdleTimer.name = "BossIdleTimer";
	bossIdleTimer.duration = myOrderDelays[myStage];
	bossIdleTimer.callback = [this]() { myUnderOrderDelay = false; myIsIdle = false; };
	Context::Get()->myTimeHandler->AddTimer(bossIdleTimer);
}

void ISTE::BossBobBehaviour::GetNewStateOrder()
{
	myDoingAnAttack = true;

	//int randomState = CU::GetRandomInt(0, (int)myAttackOptions[myStage].size() - 1); // Random Attacks

	if (myAttackInt >= myAttackOptions[myStage].size()) // Controlled Attacks
		myAttackInt = 0;

	myState = (BossState)myAttackOptions[myStage][myAttackInt];

	myAttackInt += 1;
}

void ISTE::BossBobBehaviour::DoStateOrder(float aDeltaTime)
{
	switch (myState)
	{
	case ISTE::BossState::eNotInFight: // Why are you here? Should not be in this one. Lol
		break;
	case ISTE::BossState::eAcid:
	{
		DoAcidSpit(aDeltaTime);
		break;
	}
	case ISTE::BossState::eStabSweep:
	{
		DoStabSweep(aDeltaTime);
		break;
	}
	case ISTE::BossState::eStabAndStuck:
	{
		DoStabAndStuck(aDeltaTime);
		break;
	}
	case ISTE::BossState::ePlzHelp:
	{
		DoRoarHelp(aDeltaTime);
		break;
	}
	default: // Something is wrong.
		break;
	}
}

void ISTE::BossBobBehaviour::DoRoarEvent()
{
	myAnimationHelper.Play(BossAnimations::eRoar);

	myActiveScene->GetComponent<AudioSource>(myHostId)->Play(0);

	std::vector<int> vfxIds;
	vfxIds.reserve(20);

	auto ph = Context::Get()->mySprite2DParticleHandler;
	//vfxIds.push_back(ph->SpawnEmitter("SplashScreen_Sparks_1_Left"));
	//vfxIds.push_back(ph->SpawnEmitter("SplashScreen_Sparks_1_Right"));
	//vfxIds.push_back(ph->SpawnEmitter("SplashScreen_Sparks_2_Left"));
	//vfxIds.push_back(ph->SpawnEmitter("SplashScreen_Sparks_2_Right"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_Red_Big_BL"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_Red_Big_BR"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_Red_Big_TL"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_Red_Big_TR"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_Red_Small_BL"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_Red_Small_BR"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_Red_Small_TL"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_Red_Small_TR"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_White_Big_BL"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_White_Big_BR"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_White_Big_TL"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_White_Big_TR"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_White_Small_BL"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_White_Small_BR"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_White_Small_TL"));
	//vfxIds.push_back(ph->SpawnEmitter("Bob_Roar_Dust_White_Small_TR"));

	CountDown timer;
	timer.duration = 2.f;
	timer.callback = [vfxIds]()
	{
		for (const int id : vfxIds)
			Context::Get()->mySprite2DParticleHandler->DeactivateEmitter(id);
	};
	Context::Get()->myTimeHandler->AddTimer(timer);
}

void ISTE::BossBobBehaviour::DoAcidSpit(float aDeltaTime)
{
	if (myAcidSpitData[myStage].AcidOnGround)
		return;

	if (myAcidSpitData[myStage].ShotFired)
		return;

	if (myAcidSpitData[myStage].DoingAnimation)
		return;

	// Acid Prep
	{
		if (!myAcidSpitData[myStage].ReadyToSpit)
		{
			myAcidSpitData[myStage].DoingAnimation = true;

			CountDown acidAnimationBoolTimer;
			acidAnimationBoolTimer.name = "BossAcidAnimationBoolTimer";
			acidAnimationBoolTimer.duration = 3.f;
			acidAnimationBoolTimer.callback = [this]() { myAcidSpitData[myStage].ReadyToSpit = true; myAcidSpitData[myStage].DoingAnimation = false; };
			Context::Get()->myTimeHandler->AddTimer(acidAnimationBoolTimer);

			CountDown acidAnimationTimer;
			acidAnimationTimer.name = "BossAcidAnimationTimer";
			acidAnimationTimer.duration = 1.5f;
			acidAnimationTimer.callback = [this]() { myAnimationHelper.Play(BossAnimations::eAcidSpit); myActiveScene->GetComponent<AudioSource>(myHostId)->Play(6); };
			Context::Get()->myTimeHandler->AddTimer(acidAnimationTimer);

			UpdateTimer rotatingToPosTimer;
			rotatingToPosTimer.name = "BossRotateToMiddlePointTimer";
			rotatingToPosTimer.duration = 1.5f;
			rotatingToPosTimer.callback = [this, copy = myStage]() {
				Rotate({ 33.0f, 3.0f, 110.f }, (myStabAndStuckData[copy].HeadForwarningTime / 2.f) * Context::Get()->myTimeHandler->GetDeltaTime());
			};
			Context::Get()->myTimeHandler->AddTimer(rotatingToPosTimer);

			return;
		}
	}

	// Acid spit spawns and moves towards to a target position.
	{
		if (myAcidSpitData[myStage].SpitsLeft <= 0)
		{
			myAcidSpitData[myStage].AcidOnGround = true;
			CountDown AcidAttackDoneTimer;
			AcidAttackDoneTimer.name = "BossAcidAttackDoneTimer";
			AcidAttackDoneTimer.duration = myAcidSpitData[myStage].DelayAfterAllShots;
			AcidAttackDoneTimer.callback = [this, copy = myStage]() {
				myAnimationHelper.Play(BossAnimations::eIdle);
				ResetAcidSpit(copy);
				myDoingAnAttack = false;
				myIsIdle = true;
			};
			Context::Get()->myTimeHandler->AddTimer(AcidAttackDoneTimer);
			return;
		}

		myAcidSpitData[myStage].ShotFired = true;
		myAcidSpitData[myStage].SpitsLeft -= 1;

		myAcidSpitData[myStage].SpitIdCount += 1;

		CU::Vec3f target = TryAcidSpit();
		CU::Vec3f middle = myAcidSpitData[myStage].AttackSource + (target - myAcidSpitData[myStage].AttackSource);
		middle.y += myAcidSpitData[myStage].ExtraHeight;

		// Spawn forwarning entity at target.
		{
			int vfxId = Context::Get()->myVFXHandler->SpawnVFX("Bob_Acid_Warning", target);

			// Delay before a new forwarning stab is created.
			CountDown delayOnStabs;
			delayOnStabs.name = "BossStabDelay";
			delayOnStabs.duration = myAcidSpitData[myStage].DelayBetweenSpits;
			delayOnStabs.callback = [this, copy = myStage]() { myAcidSpitData[copy].ShotFired = false; };
			Context::Get()->myTimeHandler->AddTimer(delayOnStabs);

			// Forwarning timer before stab is created
			CountDown forwarningTimer;
			forwarningTimer.name = "BossForwarningStabTimer";
			forwarningTimer.duration = myAcidSpitData[myStage].PrewarningTime;
			forwarningTimer.callback = [this, vfxId, copy = myStage]() { Context::Get()->myVFXHandler->KillVFX(vfxId); };
			Context::Get()->myTimeHandler->AddTimer(forwarningTimer);
		}

		// Spawn spit from boss.
		{
			int vfxId = Context::Get()->myVFXHandler->SpawnVFX("Bob_Acid_Spit", myAcidSpitData[myStage].AttackSource);

			// Movement Timer
			UpdateTimer spitMovementTimer;
			std::string timeName = "BossSpitMovementTimer" + std::to_string(myAcidSpitData[myStage].SpitIdCount);
			spitMovementTimer.name = timeName;
			spitMovementTimer.duration = myAcidSpitData[myStage].PrewarningTime;
			spitMovementTimer.callback = [this, vfxId, target, middle, timeName, copy = myStage]()
			{
				float percentage = Context::Get()->myTimeHandler->GetTime(timeName) / myAcidSpitData[copy].PrewarningTime;

				CU::Vec3f result = CU::BezierQuadratic(myAcidSpitData[copy].AttackSource, middle, target, percentage);

				Context::Get()->myVFXHandler->SetVFXPosition(vfxId, result);
			};
			Context::Get()->myTimeHandler->AddTimer(spitMovementTimer);

			// How long the spit lasts
			CountDown spitStayTimer;
			spitStayTimer.name = "BossSpitStayTimer";
			spitStayTimer.duration = myAcidSpitData[myStage].PrewarningTime + 0.01f;
			spitStayTimer.callback = [this, vfxId, target, copy = myStage]()
			{
				Context::Get()->myVFXHandler->KillVFX(vfxId);
				CreatePuddle(target, copy);
			};
			Context::Get()->myTimeHandler->AddTimer(spitStayTimer);
		}
	}
}

void ISTE::BossBobBehaviour::ResetAcidSpit()
{
	myAcidSpitData[myStage].SpitsLeft = myAcidSpitData[myStage].SpitCount;
	myAcidSpitData[myStage].ShotFired = false;
	myAcidSpitData[myStage].AcidOnGround = false;
	myAcidSpitData[myStage].DoingAnimation = false;
	myAcidSpitData[myStage].ReadyToSpit = false;
}

void ISTE::BossBobBehaviour::ResetAcidSpit(int aStage)
{
	myAcidSpitData[aStage].SpitsLeft = myAcidSpitData[aStage].SpitCount;
	myAcidSpitData[aStage].ShotFired = false;
	myAcidSpitData[aStage].AcidOnGround = false;
	myAcidSpitData[aStage].DoingAnimation = false;
	myAcidSpitData[aStage].ReadyToSpit = false;
}

void ISTE::BossBobBehaviour::CreatePuddle(CU::Vec3f aTarget, int aStage)
{
	myAcidSpitData[aStage].AcidIdCount += 1;

	std::string id = "BossPuddleId" + std::to_string(myAcidSpitData[aStage].AcidIdCount);

	// new entity and assign all components
	myAttackIds[id] = myActiveScene->NewEntity();
	SphereTriggerComponent* attackTrigger = myActiveScene->AssignComponent<SphereTriggerComponent>(myAttackIds[id]);
	TransformComponent* attackTransform = myActiveScene->AssignComponent<TransformComponent>(myAttackIds[id]);
	AttackValueComponent* attack = myActiveScene->AssignComponent<AttackValueComponent>(myAttackIds[id]);

	// Set all values
	attack->myAttackValue = myAcidSpitData[aStage].Damage;
	attack->myTag = "EnemyAttack";

	attackTransform->myPosition = aTarget;

	attackTrigger->myRadius = myAcidSpitData[aStage].SizePerPuddle;

	int vfxId = Context::Get()->myVFXHandler->SpawnVFX("Bob_Acid_Pool", aTarget);

	EntityID decalId = myActiveScene->NewEntity();
	TransformComponent* decalTransform = myActiveScene->AssignComponent<TransformComponent>(decalId);
	decalTransform->myPosition = attackTransform->myPosition;
	decalTransform->myEuler.SetRotation({ 0, CU::GetRandomFloat(0.f, 360.f), 0 });
	decalTransform->myScale *= 0.1f;
	DecalComponent* decalDecal = myActiveScene->AssignComponent<DecalComponent>(decalId);
	MaterialComponent* decalMaterial = myActiveScene->AssignComponent<MaterialComponent>(decalId);
	std::wstring decalPath;
	switch (CU::GetRandomInt(1, 3))
	{
	case 1: decalPath = L"../Assets/Sprites/Decals/bobAcid_1.dds"; break;
	case 2: decalPath = L"../Assets/Sprites/Decals/bobAcid_2.dds"; break;
	case 3: decalPath = L"../Assets/Sprites/Decals/bobAcid_3.dds"; break;
	}
	decalMaterial->myTextures[0][ALBEDO_MAP] = Context::Get()->myTextureManager->LoadTexture(decalPath).myValue;
	decalDecal->myLerpValues = { 0, 1, 1 };
	
	UpdateTimer decalScaleTimer;
	decalScaleTimer.duration = 0.2f;
	decalScaleTimer.callback = [decalId]() {
		TransformComponent* transform = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<TransformComponent>(decalId);
		const float addScale = Context::Get()->myTimeHandler->GetDeltaTime() * 35.f;
		transform->myScale.x += addScale;
		transform->myScale.y += addScale;
		transform->myScale.z += addScale;

	};
	Context::Get()->myTimeHandler->AddTimer(decalScaleTimer);
	
	CountDown puddleStayTimer;
	puddleStayTimer.name = "BossPuddleStayTimer";
	puddleStayTimer.duration = myAcidSpitData[aStage].StayTime;
	puddleStayTimer.callback = [this, id, vfxId, copy = aStage, decalId]() {
		myActiveScene->DestroyEntity(myAttackIds[id]);
		myAttackIds.erase(id);
		Context::Get()->myVFXHandler->DeactivateVFX(vfxId);

		UpdateTimer decalFadeTimer;
		decalFadeTimer.duration = 1.f;
		decalFadeTimer.callback = [decalId]() {
			DecalComponent* decal = Context::Get()->mySceneHandler->GetActiveScene().GetComponent<DecalComponent>(decalId);
			decal->myLerpValues.x += Context::Get()->myTimeHandler->GetDeltaTime() / 3.f;
		};
		Context::Get()->myTimeHandler->AddTimer(decalFadeTimer);
	};
	Context::Get()->myTimeHandler->AddTimer(puddleStayTimer);
}

CU::Vec3f ISTE::BossBobBehaviour::TryAcidSpit()
{
	// Random Pos
	CU::Vec3f position = { myPlayerTransform->myPosition.x + CU::GetRandomFloat(-myAcidSpitData[myStage].RandomAcidSpitPlacementRange,myAcidSpitData[myStage].RandomAcidSpitPlacementRange),
		myPlayerTransform->myPosition.y,
		myPlayerTransform->myPosition.z + CU::GetRandomFloat(-myAcidSpitData[myStage].RandomAcidSpitPlacementRange, myAcidSpitData[myStage].RandomAcidSpitPlacementRange) };

	NavMesh::NavMesh* navMesh = &myActiveScene->GetNavMesh();

	if (navMesh == nullptr || navMesh->triangles.size() == 0)
		return myPlayerTransform->myPosition;

	if (NavMesh::FindTriangleIndexFromPos(position, *navMesh) < 0)
		return TryAcidSpit();

	return position;
}

void ISTE::BossBobBehaviour::DoStabSweep(float aDeltaTime)
{
	if (myStabSweepData[myStage].PreparingSweep)
		return;

	if (myStabSweepData[myStage].StabbyTime) // Move with delta time and rotation. Also Update warning pos and attack pos. Use Sin and Cos.
	{
		if (myStabSweepData[myStage].DoingAttack)
		{
			Rotate(myActiveScene->GetComponent<TransformComponent>(myStabSweepData[myStage].AttackStayID)->myPosition, myRotationSpeed * aDeltaTime);
			return;
		}

		myAnimationHelper.Play(BossAnimations::eStabSweep);
		myActiveScene->GetComponent<AudioSource>(myHostId)->Play(4);

		myStabSweepData[myStage].DoingAttack = true;

		// Create attack entity.
		myStabSweepData[myStage].AttackStayID = myActiveScene->NewEntity();

		// Assign all components
		SphereTriggerComponent* attackTrigger = myActiveScene->AssignComponent<SphereTriggerComponent>(myStabSweepData[myStage].AttackStayID);
		TransformComponent* attackTransfrom = myActiveScene->AssignComponent<TransformComponent>(myStabSweepData[myStage].AttackStayID);
		AttackValueComponent* attack = myActiveScene->AssignComponent<AttackValueComponent>(myStabSweepData[myStage].AttackStayID);

		// Set all values
		attack->myAttackValue = myStabSweepData[myStage].Damage;
		attack->myTag = "EnemyAttack";

		attackTransfrom->myPosition = myStabSweepData[myStage].StartPoint;

		attackTrigger->myRadius = myStabSweepData[myStage].SizeOnSweep;

		int vfxId = Context::Get()->myVFXHandler->SpawnVFX("Bob_Sweep_Active", myStabSweepData[myStage].StartPoint);

		// Movement Timer
		UpdateTimer sweepMovementTimer;
		sweepMovementTimer.name = "BossSweepMovementTimer";
		sweepMovementTimer.duration = myStabSweepData[myStage].Speed;
		sweepMovementTimer.callback = [this, attackTransfrom, vfxId, copy = myStage]()
		{
			float percentage = Context::Get()->myTimeHandler->GetTime("BossSweepMovementTimer") / myStabSweepData[copy].Speed;

			CU::Vec3f result = CU::BezierQuadratic(myStabSweepData[copy].StartPoint, myStabSweepData[copy].MiddlePoint, myStabSweepData[copy].EndPoint, percentage);

			attackTransfrom->myPosition = result;
			Context::Get()->myVFXHandler->SetVFXPosition(vfxId, result);
		};
		Context::Get()->myTimeHandler->AddTimer(sweepMovementTimer);

		// Spawn Claw Timer
		StepTimer clawTimer;
		clawTimer.name = "BossSweepClawTimer";
		clawTimer.delay = 0.5f;
		clawTimer.callback = [attackTransfrom]()
		{
			Context::Get()->myVFXHandler->SpawnVFX("Bob_Stab_Claw", attackTransfrom->myPosition);
		};
		Context::Get()->myTimeHandler->AddTimer(clawTimer);

		// How long the attack lasts
		CountDown sweepStayTimer;
		sweepStayTimer.name = "BossSweepStayTimer";
		sweepStayTimer.duration = myStabSweepData[myStage].Speed + 0.01f;
		sweepStayTimer.callback = [this, vfxId, copy = myStage]()
		{
			myActiveScene->GetComponent<AudioSource>(myHostId)->Stop(4);
			myDoingAnAttack = false;
			myActiveScene->DestroyEntity(myStabSweepData[copy].AttackStayID);
			Context::Get()->myVFXHandler->DeactivateVFX(vfxId);
			myAnimationHelper.Play(BossAnimations::eIdle);
			ResetStabSweep(copy);
			myIsIdle = true;
			Context::Get()->myTimeHandler->RemoveTimer("BossSweepClawTimer");
		};
		Context::Get()->myTimeHandler->AddTimer(sweepStayTimer);

		return;
	}

	// Pre Attack, spawn forwarning and timer.
	{
		myStabSweepData[myStage].PreparingSweep = true;

		TryStabSweep();

		int vfxId = Context::Get()->myVFXHandler->SpawnVFX("Bob_Sweep_Warning", myStabSweepData[myStage].StartPoint);

		myActiveScene->GetComponent<AudioSource>(myHostId)->Play(5);

		// Forwarning timer before stab is created
		CountDown forwarningTimer;
		forwarningTimer.name = "BossForwarningStabSweepTimer";
		forwarningTimer.duration = myStabSweepData[myStage].ForwarningTime;
		forwarningTimer.callback = [this, vfxId, copy = myStage]() {
			myStabSweepData[copy].StabbyTime = true;
			myStabSweepData[copy].PreparingSweep = false;
			Context::Get()->myVFXHandler->KillVFX(vfxId);
		};
		Context::Get()->myTimeHandler->AddTimer(forwarningTimer);

		UpdateTimer rotatingToPosTimer;
		rotatingToPosTimer.name = "BossRotateToStabSweepPointTimer";
		rotatingToPosTimer.duration = myStabSweepData[myStage].ForwarningTime / 2.f;
		rotatingToPosTimer.callback = [this, copy = myStage]() {
			Rotate(myStabSweepData[myStage].StartPoint, (myStabSweepData[copy].ForwarningTime / 2.f) * Context::Get()->myTimeHandler->GetDeltaTime());
		};
		Context::Get()->myTimeHandler->AddTimer(rotatingToPosTimer);
	}
}

void ISTE::BossBobBehaviour::ResetStabSweep()
{
	myStabSweepData[myStage].StabbyTime = false;
	myStabSweepData[myStage].DoingAttack = false;
	myStabSweepData[myStage].PreparingSweep = false;
}

void ISTE::BossBobBehaviour::ResetStabSweep(int aStage)
{
	myStabSweepData[aStage].StabbyTime = false;
	myStabSweepData[aStage].DoingAttack = false;
	myStabSweepData[aStage].PreparingSweep = false;
}

void ISTE::BossBobBehaviour::TryStabSweep()
{
	if ((myStabSweepData[myStage].LeftPoint - myPlayerTransform->myPosition).Length() < (myStabSweepData[myStage].RightPoint - myPlayerTransform->myPosition).Length())
	{
		myStabSweepData[myStage].StartPoint = myStabSweepData[myStage].LeftPoint;
		myStabSweepData[myStage].EndPoint = myStabSweepData[myStage].RightPoint;
	}
	else
	{
		myStabSweepData[myStage].StartPoint = myStabSweepData[myStage].RightPoint;
		myStabSweepData[myStage].EndPoint = myStabSweepData[myStage].LeftPoint;
	}
}

void ISTE::BossBobBehaviour::DoStabAndStuck(float aDeltaTime)
{
	if (myStabAndStuckData[myStage].CalmBeforeTheStorm)
		return;

	if (myStabAndStuckData[myStage].DoHeadAttack) // This mean it's done with all it's stab attacks and does the big head slam.
	{
		if (myStabAndStuckData[myStage].PreparingHead)
			return;

		if (myStabAndStuckData[myStage].IsStuck)
			return;

		PreHead();

		return;
	}

	// This is all stab stuff below.
	{
		if (!myStabAndStuckData[myStage].AnimationPlayed)
		{
			myActiveScene->GetComponent<AudioSource>(myHostId)->Play(7);
			myAnimationHelper.Play(BossAnimations::eStabs);
		}

		if (myStabAndStuckData[myStage].StabsLeft == 0) // Uh oh, it's heading time. (Not sorry)
		{
			myStabAndStuckData[myStage].DoHeadAttack = true;
			myStabAndStuckData[myStage].StabsLeft = myStabAndStuckData[myStage].StabCount;
			myStabAndStuckData[myStage].ReadyToStab = true;
			myStabAndStuckData[myStage].CalmBeforeTheStorm = true;

			// Delay before a new forwarning stab is created.
			CountDown delayBeforeHead;
			delayBeforeHead.name = "BossCalmBeforeHead";
			delayBeforeHead.duration = myStabAndStuckData[myStage].DelayBeforeHeadAttack;
			delayBeforeHead.callback = [this, copy = myStage]() { myStabAndStuckData[copy].CalmBeforeTheStorm = false; };
			Context::Get()->myTimeHandler->AddTimer(delayBeforeHead);

			return;
		}

		if (!myStabAndStuckData[myStage].ReadyToStab) // A stab is already on the way.
			return;

		PreStab();

	}
}

void ISTE::BossBobBehaviour::ResetStabAndStuck()
{
	myStabAndStuckData[myStage].StabsLeft = myStabAndStuckData[myStage].StabCount;
	myStabAndStuckData[myStage].DoHeadAttack = false;
	myStabAndStuckData[myStage].ReadyToStab = true;
	myStabAndStuckData[myStage].IsStuck = false;
	myStabAndStuckData[myStage].PreparingHead = false;
	myStabAndStuckData[myStage].CalmBeforeTheStorm = false;
}

void ISTE::BossBobBehaviour::ResetStabAndStuck(int aStage)
{
	myStabAndStuckData[aStage].StabsLeft = myStabAndStuckData[aStage].StabCount;
	myStabAndStuckData[aStage].DoHeadAttack = false;
	myStabAndStuckData[aStage].ReadyToStab = true;
	myStabAndStuckData[aStage].IsStuck = false;
	myStabAndStuckData[aStage].PreparingHead = false;
	myStabAndStuckData[aStage].CalmBeforeTheStorm = false;
}

void ISTE::BossBobBehaviour::PreStab()
{
	myStabAndStuckData[myStage].StabsLeft -= 1;

	myStabAndStuckData[myStage].UnderStabDelay = true;
	myStabAndStuckData[myStage].ReadyToStab = false;

	CU::Vec3f target = TryStab();

	int vfxId = Context::Get()->myVFXHandler->SpawnVFX("Bob_Stab_Warning", target);

	// Delay before a new forwarning stab is created.
	CountDown delayOnStabs;
	delayOnStabs.name = "BossStabDelay";
	delayOnStabs.duration = myStabAndStuckData[myStage].DelayBetweenStabs;
	delayOnStabs.callback = [this, copy = myStage]() { myStabAndStuckData[copy].ReadyToStab = true; };
	Context::Get()->myTimeHandler->AddTimer(delayOnStabs);

	// Forwarning timer before stab is created
	CountDown forwarningTimer;
	forwarningTimer.name = "BossForwarningStabTimer";
	forwarningTimer.duration = myStabAndStuckData[myStage].StabForwarningTime;
	forwarningTimer.callback = [this, vfxId, target, copy = myStage]() {
		Stab(target, copy);
		Context::Get()->myVFXHandler->KillVFX(vfxId);
	};
	Context::Get()->myTimeHandler->AddTimer(forwarningTimer);
}

void ISTE::BossBobBehaviour::Stab(CU::Vec3f aTarget, int aStage)
{
	myStabAndStuckData[aStage].StabIdCount += 1;

	if (!myActiveScene->GetComponent<AudioSource>(myHostId)->IsPlaying(7))
	{
		myActiveScene->GetComponent<AudioSource>(myHostId)->Play(7);
	}

	std::string id = "BossStabId" + std::to_string(myStabAndStuckData[aStage].StabIdCount);

	// new entity and assign all components
	myAttackIds[id] = myActiveScene->NewEntity();
	SphereTriggerComponent* attackTrigger = myActiveScene->AssignComponent<SphereTriggerComponent>(myAttackIds[id]);
	TransformComponent* attackTransform = myActiveScene->AssignComponent<TransformComponent>(myAttackIds[id]);
	AttackValueComponent* attack = myActiveScene->AssignComponent<AttackValueComponent>(myAttackIds[id]);

	// Set all values
	attack->myAttackValue = myStabAndStuckData[aStage].StabDamage;
	attack->myTag = "EnemyAttack";

	attackTransform->myPosition = aTarget;

	attackTrigger->myRadius = myStabAndStuckData[aStage].SizePerStabAttack;

	int vfxId = Context::Get()->myVFXHandler->SpawnVFX("Bob_Stab_Claw", aTarget);

	CountDown stabStayTimer;
	stabStayTimer.name = "BossStabStayTimer";
	stabStayTimer.duration = myStabAndStuckData[aStage].AttackStayTime;
	stabStayTimer.callback = [this, id, vfxId, copy = aStage]() {
		myActiveScene->DestroyEntity(myAttackIds[id]);
		myAttackIds.erase(id);
		//Context::Get()->myVFXHandler->DeactivateVFX(vfxId);
	};
	Context::Get()->myTimeHandler->AddTimer(stabStayTimer);
}

CU::Vec3f ISTE::BossBobBehaviour::TryStab()
{
	// Random Pos
	CU::Vec3f position = { myPlayerTransform->myPosition.x + CU::GetRandomFloat(-myStabAndStuckData[myStage].RandomStabPlacementRange,myStabAndStuckData[myStage].RandomStabPlacementRange),
		myPlayerTransform->myPosition.y,
		myPlayerTransform->myPosition.z + CU::GetRandomFloat(-myStabAndStuckData[myStage].RandomStabPlacementRange, myStabAndStuckData[myStage].RandomStabPlacementRange) };

	NavMesh::NavMesh* navMesh = &myActiveScene->GetNavMesh();

	if (navMesh == nullptr || navMesh->triangles.size() == 0)
		return myPlayerTransform->myPosition;

	if (NavMesh::FindTriangleIndexFromPos(position, *navMesh) < 0)
		return TryStab();

	return position;
}

void ISTE::BossBobBehaviour::PreHead()
{
	myStabAndStuckData[myStage].PreparingHead = true;

	CU::Vec3f target = myStabAndStuckData[myStage].HeadHitPoint;

	int vfxId = Context::Get()->myVFXHandler->SpawnVFX("Bob_Stuck_Warning", target);

	// Forwarning timer before head is used
	CountDown forwarningTimer;
	forwarningTimer.name = "BossForwarningHeadTimer";
	forwarningTimer.duration = myStabAndStuckData[myStage].HeadForwarningTime;
	forwarningTimer.callback = [this, vfxId, target, copy = myStage]() {
		if (!myStats->myIsDead)
			myAnimationHelper.Play(BossAnimations::eHeadbang);

		UseHead(target, copy);
		Context::Get()->myVFXHandler->KillVFX(vfxId);
	};
	Context::Get()->myTimeHandler->AddTimer(forwarningTimer);

	UpdateTimer rotatingToPosTimer;
	rotatingToPosTimer.name = "BossRotateToHeadPointTimer";
	rotatingToPosTimer.duration = myStabAndStuckData[myStage].HeadForwarningTime / 2.f;
	rotatingToPosTimer.callback = [this, target, copy = myStage]() {
		Rotate(target, (myStabAndStuckData[copy].HeadForwarningTime / 2.f) * Context::Get()->myTimeHandler->GetDeltaTime());
	};
	Context::Get()->myTimeHandler->AddTimer(rotatingToPosTimer);

	CountDown headbangAnimTimer;
	headbangAnimTimer.name = "BossHeadbangAnimTimer";
	headbangAnimTimer.duration = myStabAndStuckData[myStage].HeadForwarningTime / 1.25f;
	headbangAnimTimer.callback = [this]() { myAnimationHelper.Play(BossAnimations::eHeadbang); };
	Context::Get()->myTimeHandler->AddTimer(headbangAnimTimer);
}

void ISTE::BossBobBehaviour::UseHead(CU::Vec3f aTarget, int aStage)
{
	myStabAndStuckData[aStage].IsStuck = true;

	myActiveScene->GetComponent<AudioSource>(myHostId)->Play(2);

	// new entity and assign all components
	EntityID headAttack = myActiveScene->NewEntity();
	SphereTriggerComponent* attackTrigger = myActiveScene->AssignComponent<SphereTriggerComponent>(headAttack);
	TransformComponent* attackTransform = myActiveScene->AssignComponent<TransformComponent>(headAttack);
	AttackValueComponent* attack = myActiveScene->AssignComponent<AttackValueComponent>(headAttack);

	// Set all values
	attack->myAttackValue = myStabAndStuckData[aStage].HeadDamage;
	attack->myTag = "EnemyAttack";

	attackTransform->myPosition = aTarget;
	int vfxId = Context::Get()->myVFXHandler->SpawnVFX("Bob_Vulnerable", aTarget);

	attackTrigger->myRadius = myStabAndStuckData[aStage].SizePerHeadAttack;

	CountDown headStayTimer;
	headStayTimer.name = "BossHeadAttackStayTimer";
	headStayTimer.duration = myStabAndStuckData[aStage].AttackStayTime;
	headStayTimer.callback = [this, headAttack, copy = aStage]() {
		myActiveScene->DestroyEntity(headAttack);

		if (!myStats->myIsDead)
			myAnimationHelper.Play(BossAnimations::eStuck);

		SphereTriggerComponent* trigger = myActiveScene->GetComponent<SphereTriggerComponent>(myHostId);
		trigger->myOffset = { 5.5f, -2.9f, -16.f };
		trigger->myRadius = 0.2f;
	};
	Context::Get()->myTimeHandler->AddTimer(headStayTimer);

	CountDown headStuckTimer;
	headStuckTimer.name = "BossHeadAttackStuckTimer";
	headStuckTimer.duration = myStabAndStuckData[aStage].StuckTime;
	headStuckTimer.callback = [this, vfxId, copy = aStage]() {
		if (!myStats->myIsDead)
			myAnimationHelper.Play(BossAnimations::eHeadStuckReverse);

		myActiveScene->GetComponent<AudioSource>(myHostId)->Play(3);

		Context::Get()->myVFXHandler->DeactivateVFX(vfxId);

		SphereTriggerComponent* trigger = myActiveScene->GetComponent<SphereTriggerComponent>(myHostId);
		trigger->myOffset = { 0.f, 0.9f, 0.f };
		trigger->myRadius = 1.f;

		CountDown headStuckAnimationTimer;
		headStuckAnimationTimer.name = "BossHeadStuckReverseAnimationTimer";
		headStuckAnimationTimer.duration = 1.f;
		headStuckAnimationTimer.callback = [this, copy]() {
			ResetStabAndStuck(copy);
			myDoingAnAttack = false;
			myIsIdle = true;
		};
		Context::Get()->myTimeHandler->AddTimer(headStuckAnimationTimer);
	};
	Context::Get()->myTimeHandler->AddTimer(headStuckTimer);

	myStabAndStuckData[aStage].PreparingHead = false;
}

void ISTE::BossBobBehaviour::DoRoarHelp(float aDeltaTime)
{
	if (!myRoarHelpData[myStage].ReadyToSpawn)
		return;

	if (myRoarHelpData[myStage].SpawnPool.empty() || myRoarHelpData[myStage].SpawnPoolSize <= 0)
		return;

	myAnimationHelper.Play(BossAnimations::eRoar);
	myActiveScene->GetComponent<AudioSource>(myHostId)->Play(1);

	myRoarHelpData[myStage].ReadyToSpawn = false;

	myRoarHelpData[myStage].LurkersLeft = CU::GetRandomUnsignedInt(myRoarHelpData[myStage].SpawnCountRangeLurker.x, myRoarHelpData[myStage].SpawnCountRangeLurker.y);
	myRoarHelpData[myStage].HuntersLeft = CU::GetRandomUnsignedInt(myRoarHelpData[myStage].SpawnCountRangeHunter.x, myRoarHelpData[myStage].SpawnCountRangeHunter.y);

	float duration = (float)(myRoarHelpData[myStage].LurkersLeft + myRoarHelpData[myStage].HuntersLeft) * myRoarHelpData[myStage].DelayBetweenSpawns;

	UpdateTimer spawnTimer;
	spawnTimer.name = "BossSpawnTimer";
	spawnTimer.duration = duration;
	spawnTimer.callback = [this, copy = myStage]()
	{
		if (myStats->myIsDead)
			return;

		if (myRoarHelpData[copy].DelayOnSpawn)
			return;

		myRoarHelpData[copy].DelayOnSpawn = true;

		const unsigned int type = CU::GetRandomUnsignedInt(1, 2);

		if (type == 1 && myRoarHelpData[copy].LurkersLeft > 0) // Random time
		{
			myRoarHelpData[copy].LurkersLeft -= 1;
			SpawnLurker(copy);
		}
		else if (type == 2 && myRoarHelpData[copy].HuntersLeft > 0) // Random time
		{
			myRoarHelpData[copy].HuntersLeft -= 1;
			SpawnHunter(copy);
		}
		else // This happens if the other one is empty, ex: LurkersLeft = 0, HuntersLeft = 2
		{
			if (myRoarHelpData[copy].LurkersLeft > 0)
			{
				myRoarHelpData[copy].LurkersLeft -= 1;
				SpawnLurker(copy);
			}
			else if (myRoarHelpData[copy].HuntersLeft > 0)
			{
				myRoarHelpData[copy].HuntersLeft -= 1;
				SpawnHunter(copy);
			}
		}

		// Delay before can spawn a new one.
		CountDown spawnDelayTimer;
		spawnDelayTimer.name = "BossSpawnDelayTimer";
		spawnDelayTimer.duration = myRoarHelpData[copy].DelayBetweenSpawns;
		spawnDelayTimer.callback = [this, copy]() { myRoarHelpData[copy].DelayOnSpawn = false; };
		Context::Get()->myTimeHandler->AddTimer(spawnDelayTimer);
	};
	Context::Get()->myTimeHandler->AddTimer(spawnTimer);

	// When the spawning is done.
	CountDown spawnDurationTimer;
	spawnDurationTimer.name = "BossSpawnDurationTimer";
	spawnDurationTimer.duration = duration;
	spawnDurationTimer.callback = [this, copy = myStage]()
	{
		if (myStats->myIsDead)
			return;

		myAnimationHelper.Play(BossAnimations::eIdle);
		ResetRoarHelp(copy);
		myDoingAnAttack = false;
		myIsIdle = true;
	};
	Context::Get()->myTimeHandler->AddTimer(spawnDurationTimer);
}

void ISTE::BossBobBehaviour::ResetRoarHelp()
{
	myRoarHelpData[myStage].ReadyToSpawn = true;
	myRoarHelpData[myStage].DelayOnSpawn = false;

	myRoarHelpData[myStage].LurkersLeft = 0;
	myRoarHelpData[myStage].HuntersLeft = 0;
}

void ISTE::BossBobBehaviour::ResetRoarHelp(int aStage)
{
	myRoarHelpData[aStage].ReadyToSpawn = true;
	myRoarHelpData[aStage].DelayOnSpawn = false;

	myRoarHelpData[aStage].LurkersLeft = 0;
	myRoarHelpData[aStage].HuntersLeft = 0;
}

void ISTE::BossBobBehaviour::SpawnLurker(int aStage)
{
	const unsigned int chosenSpawn = CU::GetRandomUnsignedInt(0, myRoarHelpData[aStage].SpawnPoolSize - 1);

	// Create attack entity.
	EntityID lurkerID = myActiveScene->NewEntity();

	// Assign all components
	TransformComponent* lurkerTransform = myActiveScene->AssignComponent<TransformComponent>(lurkerID);
	myActiveScene->AssignComponent<EnemyStatsComponent>(lurkerID);
	TriggerComponent* trigger = myActiveScene->AssignComponent<TriggerComponent>(lurkerID);
	trigger->myOffset = { 0.f, 0.9f, 0.f };
	trigger->mySize = { 0.6f, 1.f, 1.5f };
	ModelComponent* model = myActiveScene->AssignComponent<ModelComponent>(lurkerID);
	MaterialComponent* mat = myActiveScene->AssignComponent<MaterialComponent>(lurkerID);
	mat->myRenderFlags = RenderFlags::Enemies;
	AudioSource* audioSource = myActiveScene->AssignBehaviour<AudioSource>(lurkerID);
	IdleEnemyBehaviour* iEB = myActiveScene->AssignBehaviour<IdleEnemyBehaviour>(lurkerID);
	iEB->SetIsActive(false);
	iEB->Init();

	model->myModelId = Context::Get()->myModelManager->LoadModel("../Assets/Models/Characters/CH_NPC_Tim_RIG.fbx").myValue;

	Model* m = Context::Get()->myModelManager->GetModel(model->myModelId);
	memcpy(mat->myTextures, m->myTextures, sizeof(TextureID) * MAX_MESH_COUNT * MAX_MATERIAL_COUNT);

	LurkerEnemyBehaviour* lEB = myActiveScene->AssignBehaviour<LurkerEnemyBehaviour>(lurkerID);
	AnimatorComponent* animator = myActiveScene->AssignComponent<AnimatorComponent>(lurkerID);

	lEB->myIdleAnim = Context::Get()->myAnimationManager->LoadAnimation(model->myModelId, "../Assets/Animations/CH_NPC_Tim_idle_ANIM.fbx");
	lEB->myMovmentAnim = Context::Get()->myAnimationManager->LoadAnimation(model->myModelId, "../Assets/Animations/CH_NPC_Tim_movement_ANIM.fbx");
	lEB->myDeadAnim = Context::Get()->myAnimationManager->LoadAnimation(model->myModelId, "../Assets/Animations/CH_NPC_Tim_dead_ANIM.fbx");
	lEB->myAttackAnim = Context::Get()->myAnimationManager->LoadAnimation(model->myModelId, "../Assets/Animations/CH_NPC_Tim_attack_ANIM.fbx");

	lEB->Init();

	lEB->myFindRadius = 100.f;

	std::ifstream soundStream("../Assets/Json/EnemySounds.json");

	assert(soundStream.good(), "Sound json for enemys could not be found file path = ../Assets/Json/EnemySounds.json");

	nlohmann::json soundJson;
	soundStream >> soundJson;
	soundStream.close();

	audioSource->LoadAndMapClip((int)LurkerSounds::eAttack, soundJson["Lurker"]["Attack"]["SoundPath"].get<std::string>(), soundJson["Lurker"]["Attack"]["Loop"].get<bool>());
	audioSource->LoadAndMapClip((int)LurkerSounds::eDeath, soundJson["Lurker"]["Death"]["SoundPath"].get<std::string>(), soundJson["Lurker"]["Death"]["Loop"].get<bool>());

	audioSource->LoadAndMapClip((int)SharedSounds::eTookDamage, soundJson["Lurker"]["TookDamage"]["SoundPath"].get<std::string>(), soundJson["Lurker"]["TookDamage"]["Loop"].get<bool>());

	lurkerTransform->myPosition = myRoarHelpData[aStage].SpawnPool[chosenSpawn];

	// Movement Timer
	UpdateTimer newLurkerMovementTimer;
	newLurkerMovementTimer.name = "BossNewLurkerMovementTimer";
	newLurkerMovementTimer.duration = myRoarHelpData[aStage].SpawnPushToMiddle;
	newLurkerMovementTimer.callback = [this, lurkerTransform, copy = aStage]()
	{
		CU::Vec3f result = lurkerTransform->myPosition + MovementMath::GetUnitVector3(lurkerTransform->myPosition, myRoarHelpData[copy].MiddlePoint) * Context::Get()->myTimeHandler->GetDeltaTime() * 1.f;

		lurkerTransform->myPosition = result;
	};
	Context::Get()->myTimeHandler->AddTimer(newLurkerMovementTimer);

	// After movement done, activate the lurker.
	CountDown activateLurkerTimer;
	activateLurkerTimer.name = "BossActivateLurkerTimer";
	activateLurkerTimer.duration = myRoarHelpData[aStage].SpawnPushToMiddle + 0.01f;
	activateLurkerTimer.callback = [this, iEB, lurkerTransform, copy = aStage]()
	{
		lurkerTransform->myPosition = { lurkerTransform->myPosition.x, myRoarHelpData[copy].MiddlePoint.y, lurkerTransform->myPosition.z };
		iEB->SetIsActive(true);
	};
	Context::Get()->myTimeHandler->AddTimer(activateLurkerTimer);
}

void ISTE::BossBobBehaviour::SpawnHunter(int aStage)
{
	const unsigned int chosenSpawn = CU::GetRandomUnsignedInt(0, myRoarHelpData[aStage].SpawnPoolSize - 1);

	// Create attack entity.
	EntityID hunterID = myActiveScene->NewEntity();

	// Assign all components
	TransformComponent* hunterTransform = myActiveScene->AssignComponent<TransformComponent>(hunterID);
	myActiveScene->AssignComponent<EnemyStatsComponent>(hunterID);
	TriggerComponent* trigger = myActiveScene->AssignComponent<TriggerComponent>(hunterID);
	trigger->myOffset = { 0, 0.9f,0 };
	trigger->mySize = { 1.2f, 1.f, 0.6f };
	ModelComponent* model = myActiveScene->AssignComponent<ModelComponent>(hunterID);
	MaterialComponent* mat = myActiveScene->AssignComponent<MaterialComponent>(hunterID);
	mat->myRenderFlags = RenderFlags::Enemies;
	AudioSource* audioSource = myActiveScene->AssignBehaviour<AudioSource>(hunterID);
	IdleEnemyBehaviour* iEB = myActiveScene->AssignBehaviour<IdleEnemyBehaviour>(hunterID);
	iEB->SetIsActive(false);
	iEB->Init();

	model->myModelId = Context::Get()->myModelManager->LoadModel("../Assets/Models/Characters/CH_NPC_Boris_RIG.fbx").myValue;

	Model* m = Context::Get()->myModelManager->GetModel(model->myModelId);
	memcpy(mat->myTextures, m->myTextures, sizeof(TextureID) * MAX_MESH_COUNT * MAX_MATERIAL_COUNT);

	HunterEnemyBehaviour* hEB = myActiveScene->AssignBehaviour<HunterEnemyBehaviour>(hunterID);
	AnimatorComponent* animator = myActiveScene->AssignComponent<AnimatorComponent>(hunterID);

	hEB->myIdleAnim = Context::Get()->myAnimationManager->LoadAnimation(model->myModelId, "../Assets/Animations/CH_NPC_Boris_idle_ANIM.fbx");
	hEB->myMovmentAnim = Context::Get()->myAnimationManager->LoadAnimation(model->myModelId, "../Assets/Animations/CH_NPC_Boris_walk_ANIM.fbx");
	hEB->myDeadAnim = Context::Get()->myAnimationManager->LoadAnimation(model->myModelId, "../Assets/Animations/CH_NPC_Boris_dead_ANIM.fbx");
	hEB->myAttackAnim = Context::Get()->myAnimationManager->LoadAnimation(model->myModelId, "../Assets/Animations/CH_NPC_Boris_shoot_ANIM.fbx");
	hEB->myPreExplodeAnim = Context::Get()->myAnimationManager->LoadAnimation(model->myModelId, "../Assets/Animations/CH_NPC_Boris_explode_ANIM.fbx");

	hEB->myExplodeModel = Context::Get()->myModelManager->LoadModel("../Assets/Animations/CH_NPC_Boris_sack_ANIM.fbx");
	hEB->myExplodeAnim = Context::Get()->myAnimationManager->LoadAnimation(hEB->myExplodeModel, "../Assets/Animations/CH_NPC_Boris_sack_ANIM.fbx");

	hEB->Init();

	hEB->myFindRadius = 100.f;

	std::ifstream soundStream("../Assets/Json/EnemySounds.json");

	assert(soundStream.good(), "Sound json for enemys could not be found file path = ../Assets/Json/EnemySounds.json");

	nlohmann::json soundJson;
	soundStream >> soundJson;
	soundStream.close();

	audioSource->LoadAndMapClip((int)HunterSounds::eAttack, soundJson["Hunter"]["Attack"]["SoundPath"].get<std::string>(), soundJson["Hunter"]["Attack"]["Loop"].get<bool>());
	audioSource->LoadAndMapClip((int)HunterSounds::ePreExplosion, soundJson["Hunter"]["PreExplosion"]["SoundPath"].get<std::string>(), soundJson["Hunter"]["PreExplosion"]["Loop"].get<bool>());
	audioSource->LoadAndMapClip((int)HunterSounds::eExplosion, soundJson["Hunter"]["Explosion"]["SoundPath"].get<std::string>(), soundJson["Hunter"]["Explosion"]["Loop"].get<bool>());

	audioSource->LoadAndMapClip((int)SharedSounds::eTookDamage, soundJson["Hunter"]["TookDamage"]["SoundPath"].get<std::string>(), soundJson["Hunter"]["TookDamage"]["Loop"].get<bool>());

	hunterTransform->myPosition = myRoarHelpData[aStage].SpawnPool[chosenSpawn];

	// Movement Timer
	UpdateTimer newHunterMovementTimer;
	newHunterMovementTimer.name = "BossNewHunterMovementTimer";
	newHunterMovementTimer.duration = myRoarHelpData[aStage].SpawnPushToMiddle;
	newHunterMovementTimer.callback = [this, hunterTransform, copy = aStage]()
	{
		CU::Vec3f result = hunterTransform->myPosition + MovementMath::GetUnitVector3(hunterTransform->myPosition, myRoarHelpData[copy].MiddlePoint) * Context::Get()->myTimeHandler->GetDeltaTime() * 1.f;

		hunterTransform->myPosition = result;
	};
	Context::Get()->myTimeHandler->AddTimer(newHunterMovementTimer);

	// After movement done, activate the lurker.
	CountDown activateHunterTimer;
	activateHunterTimer.name = "BossActivateHunterTimer";
	activateHunterTimer.duration = myRoarHelpData[aStage].SpawnPushToMiddle + 0.01f;
	activateHunterTimer.callback = [this, iEB, hunterTransform, copy = aStage]()
	{
		hunterTransform->myPosition = { hunterTransform->myPosition.x, myRoarHelpData[copy].MiddlePoint.y, hunterTransform->myPosition.z };
		iEB->SetIsActive(true);
	};
	Context::Get()->myTimeHandler->AddTimer(activateHunterTimer);
}