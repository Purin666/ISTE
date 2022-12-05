#pragma once
#include "Behaviour.h"

// ISTE
#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/Graphics/Camera.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"

// CU
#include "ISTE/Math/Vec3.h"
#include "ISTE/CU/RayFromScreen.h"

// stdlib
#include <queue>
#include <map>
#include <random>

#include "ISTE/Graphics/Helper/AnimationHelper.h"


enum class PlayerAnimations
{
	eIdle,
	eMovement,
	eDead,
	eThrowAbility,
	eMeleeAttack,
	eSpell,
	ePlayerBag,
	eCount
};

namespace ISTE
{
	enum class PlayerAbility
	{
		ePrimary,
		eSecondary,
		eMagicArmor,
		eTeleport,
		eAOEAtPlayer,
		eAOEAtTarget,
		
		eCount
	};

	enum class PlayerSounds
	{
		eAOE,
		eDeath,
		eWalking,
		eLevelUp,
		eTeleport,
		eMagicArmor,
		eTookDamage,
		eAOESIMCharge,
		ePrimaryAttack,
		eAOESIMExplode,
		eSecondaryAttack,
	};

	struct Context;

	class AudioSource;
	class LocalStepTimer;

	class PlayerBehaviour : public Behaviour
	{
	public:
		~PlayerBehaviour();
		void Init() override;
		void Update(float) override;

		void OnTrigger(EntityID aId) override;

		inline TransformComponent& GetCameraPositioning() { return myCameraPositioning; }
		inline Camera& GetCamera() { return myCamera; }

		inline void SetIsActive(bool aStatement) { myIsActive = aStatement; myAnimationHelper.Play(PlayerAnimations::eIdle); }

		inline const float GetHealth()				{ return myHealth; }
		inline const float GetMana()				{ return myMana; }
		inline const bool GetArmorUnlocked()		{ return myArmorUnlocked; }
		inline const bool GetTeleportUnlocked()		{ return myTeleportUnlocked; }
		inline const bool GetAoEDoTUnlocked()		{ return myAoEDoTUnlocked; }
		inline const bool GetUltimateUnlocked()		{ return myUltimateUnlocked; }
		inline const int GetExperience()			{ return myExperience; }
		void UpdateWithDatabase(CU::Database<true>& aBehaviourDatabase) override;
		void UpdateWeapon();

	private:
		friend class PlayerBuilder;
		friend class PlayerTool;
		friend class State_HUD; // TODO:: rework this later // Mathias

		void DoAbility(PlayerAbility anAbility);
		void Move(float aDeltaTime);
		void BuildPath();
		void Reset();
		void InitAbilities();
		void InitEvents();
		void InitAnimations();
		void InitAnimationLerpSpeeds();
		void PrintDamageText(float aDamageValue);
		void GainExperience(const int someExperience);

		// Ability Functions
		void PrimaryAttack();
		void SecondaryAttack();
		void MagicArmor();
		void Teleport();
		void Teleportation(TransformComponent* aTransform, CU::Vec3f aDestination);
		void AoEDoT();
		void Ultimate();

		void InitMoveDistanceTimer();

		std::mt19937 myRandomEngine;

		std::uniform_real_distribution<float> myRandomYText;
		std::uniform_real_distribution<float> myRandomXText;

		ISTE::Context* myCtx;

		int myExperience = 0;
		int myExperienceRequired = 1;
		bool myHaveGainedLevel = false;

		int myUpdateDelay = 20;
		int myUpdateCounter = 0;
		int myDamageTextCount = 0;

		float mySpeed;
		float myHealth;
		float myHealthMax;
		float myHealthReg;
		float myMana;
		float myManaMax;
		float myManaReg;

		// Primary
		float myPrimaryDamage;
		float myPrimaryAttackRange;
		float myPrimaryAttackSpeed;
		float myPrimaryAttackStayrate;

		//secondary
		std::map<std::string, EntityID> myProjectileIds;
		int myProjectileCount;

		float mySecondaryDamage = 10;
		float mySecondaryAttackStay = 5;
		float mySecondaryAttackMovmentSpeed = 2;
		float mySecondaryAttackCost = 10;

		float myStalTime = 0.1f;
		float myStalTimer = 0;
		bool myIsStaled = false;

		// Magic Armor
		float myArmorCooldown;
		float myArmorCost;
		float myArmorDuration;
		bool myArmored = false;
		bool myArmorUnlocked = false;

		// Teleport
		float myTeleportCooldown;
		float myTeleportCost;
		float myMaxTeleportDistance;
		bool myTeleportUnlocked = false;

		// AoEDoT
		float myAoEDoTCooldown;
		float myAoEDoTCost;
		float myAoEDoTDamage;
		float myAoEDoTDuration;
		float myAoEDoTRadius;
		bool myAoEDoTUnlocked = false;

		//Ultimate
		float myUltimateCooldown;
		float myUltimateCost;
		float myUltimateDamage;
		float myUltimateRadius;
		float myUltimateRange;
		float myUltimateHealthRegen;
		bool myUltimateUnlocked = false;

		bool myHasAnOrder;
		bool myIsDead;
		bool myOnIFrameMode;
		bool myCanUsePrimary = true; // For Primary attack
		bool myCanUseArmor = true;
		bool myCanUseTeleport = true; // For Teleport ability
		bool myCanUseAoEDoT = true;
		bool myDoTUsed = false;
		bool myCanUseUltimate = true;
		bool myIsActive = true;

		std::map<std::string, EntityID> myDamageTextIds;
		std::map<std::string, EntityID> myAttacksIds; // All attacks use this.

		std::queue<CU::Vec3f> myOrders;

		ScreenHelper myScreenHelper;

		Camera myCamera;

		TransformComponent myCameraPositioning;

		// VFX that follows the player, saving variables for repositioning before we can attach them to an entity // Mathias
		int myMagicArmorVFX		= -1;
		int myAreaOfEffectVFX	= -1;
		int myPrimaryAttackVFX	= -1;
		int myLevelUpVFX		= -1;
		//
		
		//testing
		int myRightHandBoneIndex = -1;
		int myLeftHandNoneIndex = -1;
		int myWeaponHoldBone = -1; 
		float myDeathTimer = 0;
		EntityID myWeaponId = INVALID_ENTITY;

		struct PlayerAnimationLerpData {  
			float myMoveToIdleSpeed;
			float myIdleToMoveSpeed;

			float myAnyToMoveSpeed;
			float myAnyToIdleSpeed;
			float myAnyToPrimaryAttackSpeed;
			float myAnyToSecondaryAttackSpeed;
			float myAnyToSpellSpeed;
			float myAnyToDeadSpeed;
			float myAnyToBagSpeed;
		} myAnimationLerpData; 


		AnimationHelper<PlayerAnimations> myAnimationHelper;

		//temp way of handling
		AnimationID myIdleId		  = AnimationID(-1);
		AnimationID myWalkId		  = AnimationID(-1);
		AnimationID myDeadId		  = AnimationID(-1);
		AnimationID myThrowAbilityId  = AnimationID(-1);
		AnimationID myMeleeAttackId   = AnimationID(-1);
		AnimationID mySpellId		  = AnimationID(-1);
		//

		AudioSource* mySource = nullptr;
		LocalStepTimer* myBuildPathTimer = nullptr;
		LocalStepTimer* myMoveDistanceTimer = nullptr;
		CU::Vec3f myLastMovePosition;
	};
}