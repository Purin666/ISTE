
#include "State_HUD.h"

#include "ISTE/Context.h"

#include "ISTE/UI/UIHandler.h"
#include "ISTE/UI/UIWorker.h"

#include "ISTE/Graphics/Resources/TextureManager.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Graphics/ComponentAndSystem/SpriteDrawerSystem.h"

#include "ISTE/CU/InputHandler.h"
#include "ISTE/CU/UtilityFunctions.hpp"

#include "ISTE/Events/EventHandler.h"
#include "ISTE/Time/TimeHandler.h"
#include "ISTE/Time/TimerDefines.h"

#include "ISTE/VFX/SpriteParticles/Sprite2DParticleHandler.h"

// to get player and boss health/mana/spells etc.
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/ECSB/PlayerBehaviour.h"
#include "ISTE/ECSB/BossBobBehaviour.h"
#include "ISTE/ComponentsAndSystems/EnemyStatsComponent.h"
#include "ISTE/Math/Math.h"

constexpr float ASPECT_RATIO = 16.f / 9.f;
constexpr float PI = 3.14159265359f;

ISTE::State_HUD::~State_HUD()
{
	myCtx->myEventHandler->RemoveCallback(EventType::PlayerGainLevel, "PlayerGainLevel");
	myCtx->myEventHandler->RemoveCallback(EventType::PlayerUsedAbility, "PlayerUsedAbility");
	myCtx->myEventHandler->RemoveCallback(EventType::PlayerTookDamage, "PlayerTookDamage");
	myCtx->myEventHandler->RemoveCallback(EventType::PlayerSpentMana, "PlayerSpentMana");
	myCtx->myEventHandler->RemoveCallback(EventType::PlayerGainExperience, "PlayerGainExperience");
	myCtx->myEventHandler->RemoveCallback(EventType::BossTookDamage, "BossTookDamage");

	// remove emitters
	myCtx->mySprite2DParticleHandler->KillEmitter(myHealthParticlesID);
	myCtx->mySprite2DParticleHandler->KillEmitter(myManaParticlesID);
	for (size_t i = 0; i < myCDVFXIDs.size(); i++)
		myCtx->mySprite2DParticleHandler->KillEmitter(myCDVFXIDs[i]);


	// reset gameplay images, this solved offsetting somewhat
	if (myPlayerHUD != nullptr)
	{
		size_t i;
		for (auto& [name, gImage] : myPlayerHUD->myGameplayImages)
		{
			if (gImage.myType == GameplayUIType::PlayerHealth)
				gImage.myImage.myCommand = myPlayerHealthImageOG;

			if (gImage.myType == GameplayUIType::PlayerMana)
				gImage.myImage.myCommand = myPlayerManaImageOG;

			if (gImage.myType == GameplayUIType::PlayerExperience)
				gImage.myImage.myCommand = myPlayerExperienceImageOG;
		}
	}
	if (myBossHUD != nullptr)
	{
		for (auto& [name, gImage] : myBossHUD->myGameplayImages)
		{
			if (gImage.myType == GameplayUIType::BossHealth)
				gImage.myImage.myCommand = myBossHealthImageOG;
		}
	}
}

void ISTE::State_HUD::Init()
{
	SetStateFlags(StateFlags_PassthruUpdate | StateFlags_PassthruRender | StateFlags_AlwaysRender);

	if (myCtx->myUIHandler->myUIDatas.count("HUD_Player"))
		myPlayerHUD = &myCtx->myUIHandler->myUIDatas.at("HUD_Player");

	const int sceneIndexTemp = myCtx->mySceneHandler->GetActiveSceneIndex();
	if (sceneIndexTemp == 6 )
	{
		if (myCtx->myUIHandler->myUIDatas.count("HUD_Boss"))
		{
			myBossHUD = &myCtx->myUIHandler->myUIDatas.at("HUD_Boss");

			Scene& scene = myCtx->mySceneHandler->GetActiveScene();
			EntityID bossId = scene.GetBossId();
			myBoss = scene.GetComponent<BossBobBehaviour>(bossId);
		}
	}

	size_t i; 
	for (auto& [name, gImage] : myPlayerHUD->myGameplayImages)
	{
		if ((size_t)gImage.myType < (size_t)GameplayUIType::PlayerHealth)
		{
			i = (size_t)gImage.myType;
			auto& fadeCom = myAbilityFades[i];
			fadeCom = gImage.myImage.myCommand;
			auto& lockCom = myLockedIcons[i];
			lockCom = gImage.myImage.myCommand;
		}
		if (gImage.myType == GameplayUIType::PlayerHealth)
		{
			myPlayerHeathImage = &gImage;
			myPlayerHealthImageOG = gImage.myImage.myCommand;
		}
		if (gImage.myType == GameplayUIType::PlayerMana)
		{
			myPlayerManaImage = &gImage;
			myPlayerManaImageOG = gImage.myImage.myCommand;
		}
		if (gImage.myType == GameplayUIType::PlayerExperience)
		{
			myPlayerExperienceImage = &gImage;
			myPlayerExperienceImageOG = gImage.myImage.myCommand;
		}
	}
	if (myBossHUD != nullptr)
	{
		for (auto& [name, gImage] : myBossHUD->myGameplayImages)
		{
			if (gImage.myType == GameplayUIType::BossHealth)
			{
				myBossHealthImage = &gImage;
				myBossHealthImageOG = gImage.myImage.myCommand;
			}
		}
	}
	// set lock icons
	const TextureID lockTextureID = myCtx->myTextureManager->LoadTexture(L"../Assets/Sprites/UI/UI HUD/Ui_lockedAbility.dds").myValue;
	for (auto& com : myLockedIcons)
	{
		com.myTextureId[ALBEDO_MAP] = lockTextureID;
		com.myColor = CU::Vec4f(1, 1, 1, 1);
		com.myScale.x /= ASPECT_RATIO;
	}
	// set fades
	const TextureID fadeTextureId = myCtx->myTextureManager->LoadTexture(L"../Assets/Sprites/fadeTexture.dds").myValue;
	for (auto& com : myAbilityFades)
	{
		com.myTextureId[ALBEDO_MAP] = fadeTextureId;
		com.myColor = CU::Vec4f(com.myColor.r, com.myColor.g, com.myColor.b, 0.85f);
		com.myScale.x /= ASPECT_RATIO;
		com.myPivot.y = 0.5f;
		com.myPosition.y += com.myScale.y * 0.5f;
	}
	// save original fades
	myAbilityFadesOG = myAbilityFades;
	// set scale of fades to 0
	myAbilityEnable = (size_t)CU::Min(myCtx->mySceneHandler->GetActiveSceneIndex(), 5);
	for (int i = myAbilityEnable; i >= 0; i--)
	{
		myLockedIcons[i].myScale = CU::Vec2f(0.f, 0.f);
	}
	for (size_t i = 0; i < myAbilityFades.size(); i++)
	{
		myAbilityFades[i].myScale = CU::Vec2f(0.f, 0.f);
	}

	// load player data
	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	EntityID playerId = scene.GetPlayerId();
	myPlayer = scene.GetComponent<PlayerBehaviour>(playerId);
	
	if (myPlayer != nullptr)
	{
		myCDTimings[0] = myPlayer->myPrimaryAttackStayrate;
		myCDTimings[1] = 0.f;
		myCDTimings[2] = myPlayer->myArmorCooldown;
		myCDTimings[3] = myPlayer->myAoEDoTCooldown;
		myCDTimings[4] = myPlayer->myTeleportCooldown;
		myCDTimings[5] = myPlayer->myUltimateCooldown;
	}

	myAbilityEnable = (size_t)myCtx->mySceneHandler->GetActiveSceneIndex();

	if (myAbilityEnable > 5)
		myAbilityEnable = 5;

	// register events
	myCtx->myEventHandler->RegisterCallback(ISTE::EventType::PlayerGainLevel, "PlayerGainLevel", [this](EntityID aLevel) { EnableAbility((int)aLevel); });
	myCtx->myEventHandler->RegisterCallback(ISTE::EventType::PlayerUsedAbility, "PlayerUsedAbility", [this](EntityID anAbility) {
		const int i = (int)anAbility;
		myElapsedTimes[i] = 0.f;
		myAbilityFades[i] = myAbilityFadesOG[i];
		myCDVFXIDs[i] = Context::Get()->mySprite2DParticleHandler->SpawnEmitter("CooldownLine");

		CU::Vec2f vfxPos = myAbilityFadesOG[i].myPosition;
		vfxPos.y -= myAbilityFadesOG[i].myScale.y * 0.5f;
		vfxPos.x *= ASPECT_RATIO;

		myCooldownTimers[i].SetOn(true);
		myCooldownTimers[i].SetDuration(myCDTimings[i]);
		myCooldownTimers[i].SetCallback([deactivateId = myCDVFXIDs[i], pos = vfxPos]() {
			Context::Get()->mySprite2DParticleHandler->KillEmitter(deactivateId);
			Context::Get()->mySprite2DParticleHandler->SpawnEmitter("CooldownDone", pos);
			});

		//CountDown timer;
		//timer.name = "Cooldown_VFX_" + std::to_string(i);
		//timer.duration = myCDTimings[i];
		//timer.callback = [ deactivateId = myCDVFXIDs[i], pos = vfxPos ]() {
		//	Context::Get()->mySprite2DParticleHandler->KillEmitter(deactivateId);
		//	Context::Get()->mySprite2DParticleHandler->SpawnEmitter("CooldownDone", pos);
		//};
		//myCtx->myTimeHandler->AddTimer(timer);
	});
	myCtx->myEventHandler->RegisterCallback(ISTE::EventType::PlayerTookDamage, "PlayerTookDamage",			[this](EntityID garbage) { myPlayerHealthLerpElapsedTime = 0.f; });
	myCtx->myEventHandler->RegisterCallback(ISTE::EventType::PlayerSpentMana, "PlayerSpentMana",			[this](EntityID garbage) { myPlayerManaLerpElapsedTime	 = 0.f; });
	myCtx->myEventHandler->RegisterCallback(ISTE::EventType::PlayerGainExperience, "PlayerGainExperience",	[this](EntityID garbage) { myExperienceLerpElapsedTime	 = 0.f; });
	myCtx->myEventHandler->RegisterCallback(ISTE::EventType::BossTookDamage, "BossTookDamage",				[this](EntityID garbage) { myBossHealthLerpElapsedTime	 = 0.f; });

	// spawn health and mana emitters
	CU::Vec2f hPos = myPlayerHeathImage->myImage.myCommand.myPosition;
	CU::Vec2f mPos = myPlayerManaImage->myImage.myCommand.myPosition;
	hPos.x *= ASPECT_RATIO;
	mPos.x *= ASPECT_RATIO;
	myHealthParticlesID = Context::Get()->mySprite2DParticleHandler->SpawnEmitter("player_health_particles", hPos);
	myManaParticlesID = Context::Get()->mySprite2DParticleHandler->SpawnEmitter("player_mana_particles", mPos);
}

void ISTE::State_HUD::Update(const float aTimeDelta)
{
	// ugh
	myElapsedTime += aTimeDelta;

	for (size_t i = 0; i < myCooldownTimers.size(); i++)
		myCooldownTimers[i].Update(aTimeDelta);

	for (size_t i = 0; i < myElapsedTimes.size(); i++)
		myElapsedTimes[i] += aTimeDelta;

	if (myPlayerHUD != nullptr)
		ProcessUI(*myPlayerHUD);

	if (myBossHUD != nullptr)
		ProcessUI(*myBossHUD);

	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	EntityID playerId = scene.GetPlayerId();
	myPlayer = scene.GetComponent<PlayerBehaviour>(playerId);
	// ugh
	if (myPlayer != nullptr)
	{
		myCDTimings[0] = myPlayer->myPrimaryAttackSpeed;
		myCDTimings[1] = 0.f;
		myCDTimings[2] = myPlayer->myArmorCooldown;
		myCDTimings[3] = myPlayer->myAoEDoTCooldown;
		myCDTimings[4] = myPlayer->myTeleportCooldown;
		myCDTimings[5] = myPlayer->myUltimateCooldown;

		ProcessAbilityFades();
		ProcessAbilityOutOfMana();
		ProcessExperience(aTimeDelta);
		ProcessHeahlthAndMana(aTimeDelta, myElapsedTime);
	}
	EntityID bossId = scene.GetBossId();
	myBoss = scene.GetComponent<BossBobBehaviour>(bossId);
	if (myBossHUD != nullptr && myBoss != nullptr)
	{
		ProcessBossHealth(aTimeDelta, myElapsedTime);
	}
}

void ISTE::State_HUD::Render()
{
	if (myPlayerHUD != nullptr)
		AddRenderCommands(*myPlayerHUD);

	if (myBossHUD != nullptr)
		AddRenderCommands(*myBossHUD);

	SpriteDrawerSystem* drawer = myCtx->mySystemManager->GetSystem<SpriteDrawerSystem>();
	for (size_t i = 0; i < myAbilityFades.size(); i++)
	{
		drawer->Add2DSpriteRenderCommand(myAbilityFades[i]);
		drawer->Add2DSpriteRenderCommand(myLockedIcons[i]);
	}
}

void ISTE::State_HUD::ProcessAbilityFades()
{
	for (size_t i = 0; i <= myAbilityEnable; i++)
	{
		if (myElapsedTimes[i] <= myCDTimings[i])
		{
			auto& com = myAbilityFades[i];
			const float t = myElapsedTimes[i] / myCDTimings[i];

			const float fromScale = myAbilityFadesOG[i].myScale.y;
			const float toScale = 0;
			com.myScale.y = -CU::Lerp(fromScale, toScale, t);

			//const float fromPos = myAbilityFadesOG[i].myPosition.y;
			//const float toPos = myAbilityFadesOG[i].myPosition.y/* + myAbilityFadesOG[i].myScale.y*/;
			//com.myPosition.y = CU::Lerp(fromPos, toPos, t);

			// move vfx
			CU::Vec2f vfxPos = com.myPosition;
			vfxPos.y += com.myScale.y;
			vfxPos.x *= ASPECT_RATIO;
			myCtx->mySprite2DParticleHandler->SetEmitterPosition(myCDVFXIDs[i], vfxPos);
		}
		else
		{
			auto& com = myAbilityFades[i];
			com.myScale.y = 0.f;
		}
	}
}

void ISTE::State_HUD::ProcessAbilityOutOfMana()
{
	const float currentMana = myPlayer->myMana;
	const CU::Vec4f white(1, 1, 1, 1);
	const CU::Vec4f red  (1, 0, 0, 1);
	for (auto& [name, image] : myPlayerHUD->myGameplayImages)
	{
		switch (image.myType)
		{
		case GameplayUIType::AbilityR:
			image.myImage.myCommand.myColor = (myPlayer->mySecondaryAttackCost < currentMana) ? white : red;
			break;
		case GameplayUIType::Ability1:
			image.myImage.myCommand.myColor = (myPlayer->myArmorCost < currentMana) ? white : red;
			break;
		case GameplayUIType::Ability2:
			image.myImage.myCommand.myColor = (myPlayer->myAoEDoTCost < currentMana) ? white : red;
			break;
		case GameplayUIType::Ability3:
			image.myImage.myCommand.myColor = (myPlayer->myTeleportCost < currentMana) ? white : red;
			break;
		case GameplayUIType::Ability4:
			image.myImage.myCommand.myColor = (myPlayer->myUltimateCost < currentMana) ? white : red;
			break;
		}
	}
}

void ISTE::State_HUD::ProcessHeahlthAndMana(const float aTimeDelta, const float elapsedTime)
{
	// health
	{
		auto& com = myPlayerHeathImage->myImage.myCommand;
		const float percent = myPlayer->myHealth / myPlayer->myHealthMax;

		const float toScale = myPlayerHealthImageOG.myScale.y;
		const float toUVScale = myPlayerHealthImageOG.myUVScale.y;
		const float toUVOffset = myPlayerHealthImageOG.myUVOffset.y;
		
		//com.myScale.y = CU::Lerp(0.f, toScale, wbt);
		//com.myUVScale.y = CU::Lerp(0.f, toUVScale, wbt);
		//com.myUVOffset.y = CU::Lerp(1.f, toUVOffset, wbt);

		myPlayerHealthLerpElapsedTime += aTimeDelta * 3.0f;
		const float blend = CU::ParametricBlend(CU::Min(myPlayerHealthLerpElapsedTime, 1.f));
		const float wbt = CU::WarfareBlend(1.f - percent + 0.0001f);
		com.myScale.y = CU::Max(0.f, CU::Min(CU::Lerp(com.myScale.y, toScale * wbt, blend), toScale));
		com.myUVScale.y = CU::Lerp(com.myUVScale.y, toUVScale * wbt, blend);
		com.myUVOffset.y = CU::Lerp(com.myUVOffset.y, 1.f - wbt, blend);

		// pulsate
		const float fromScale = myPlayerHealthImageOG.myScale.x;
		const float pulsate = 0.5f + 0.5f * std::sin(elapsedTime * 3.f);
		com.myScale.x = CU::Lerp(fromScale, fromScale + 0.03f, pulsate);
	}
	// mana
	{
		auto& com = myPlayerManaImage->myImage.myCommand;
		const float percent = myPlayer->myMana / myPlayer->myManaMax;

		const float toScale = myPlayerManaImageOG.myScale.y;
		const float toUVScale = myPlayerManaImageOG.myUVScale.y;
		const float toUVOffset = myPlayerHealthImageOG.myUVOffset.y;

		//com.myScale.y = CU::Lerp(0.f, toScale, percent);
		//com.myUVScale.y = CU::Lerp(0.f, toUVScale, percent);
		//com.myUVOffset.y = CU::Lerp(1.f, toUVOffset, percent);

		myPlayerManaLerpElapsedTime += aTimeDelta * 3.0f;
		const float blend = CU::ParametricBlend(CU::Min(myPlayerManaLerpElapsedTime, 1.f));
		com.myScale.y = CU::Min(CU::Lerp(com.myScale.y, toScale * percent, blend), toScale);
		com.myUVScale.y = CU::Lerp(com.myUVScale.y, toUVScale * percent, blend);
		com.myUVOffset.y = CU::Lerp(com.myUVOffset.y, 1.f - percent, blend);

		// pulsate
		const float fromScaleX = myPlayerHealthImageOG.myScale.x;
		const float pulsate = 0.5f + 0.5f * std::sin(elapsedTime * 3.f);
		com.myScale.x = CU::Lerp(fromScaleX, fromScaleX + 0.03f, pulsate);
	}
}

void ISTE::State_HUD::ProcessExperience(const float aTimeDelta)
{
	auto& com = myPlayerExperienceImage->myImage.myCommand;
	const float percent = (float)myPlayer->myExperience / (float)myPlayer->myExperienceRequired;

	const float toScale = myPlayerExperienceImageOG.myScale.x;
	const float toUVScale = myPlayerExperienceImageOG.myUVScale.x;

	//com.myScale.x = CU::Lerp(0.f, toScale, percent);
	//com.myUVScale.x = CU::Lerp(0.f, toUVScale, percent);
	
	myExperienceLerpElapsedTime += aTimeDelta * 0.5f;
	const float blend = CU::ParametricBlend(CU::Min(myExperienceLerpElapsedTime, 1.f));
	com.myScale.x = CU::Min(CU::Lerp(com.myScale.x, toScale * percent, blend), toScale);
	com.myUVScale.x = CU::Lerp(com.myUVScale.x, toUVScale * percent, blend);
}

void ISTE::State_HUD::ProcessBossHealth(const float aTimeDelta, const float elapsedTime)
{
	auto& com = myBossHealthImage->myImage.myCommand;
	const float percent = myBoss->myStats->myHealth / myBoss->myStats->myMaxHealth;

	const float toScale = myBossHealthImageOG.myScale.x;
	const float toUVScale = myBossHealthImageOG.myUVScale.x;
	
	com.myScale.x = CU::Lerp(0.f, toScale, percent);
	com.myUVScale.x = CU::Lerp(0.f, toUVScale, percent);

	//myBossHealthLerpElapsedTime += aTimeDelta * 0.5f;
	//const float blend = CU::ParametricBlend(CU::Min(myBossHealthLerpElapsedTime, 1.f));
	//com.myScale.x = CU::Min(CU::Lerp(com.myScale.x, toScale * percent, blend), toScale);
	//com.myUVScale.x = CU::Lerp(com.myUVScale.x, toUVScale * percent, blend);
	
	// pulsate
	//const float fromScale = myBossHealthImageOG.myScale.y;
	//const float pulsate = 0.5f + 0.5f * std::sin(elapsedTime * 3.f);
	//com.myScale.y = CU::Lerp(fromScale, fromScale + 0.1f, pulsate);
}

void ISTE::State_HUD::EnableAbility(const int aLevel)
{
	myAbilityEnable = aLevel + 1;

	if (myAbilityEnable > 5)
		myAbilityEnable = 5;

	myLockedIcons[myAbilityEnable].myColor.a = 0.f;
	myAbilityFades[myAbilityEnable].myScale.y = 0.f;
	
	CU::Vec2f pos = myAbilityFadesOG[myAbilityEnable].myPosition;
	pos.x *= ASPECT_RATIO;
	myCtx->mySprite2DParticleHandler->SpawnEmitter("Ability_Unlocked", pos);
}
