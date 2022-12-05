#pragma once
#include "Behaviour.h"
#include "ISTE/Math/Vec3.h"
#include "ISTE/Graphics/Helper/AnimationHelper.h"
#include "BobStructs.h"

#include "Json/json.hpp"

#include <string>
#include <vector>
#include <map>

namespace ISTE
{
	class Scene;
	struct TransformComponent;
	struct EnemyStatsComponent;

	enum class BossState
	{
		eNotInFight, // 0 : Är just nu inte activerad. OBS Använd inte inom "attackoptions"
		eAcid, // 1 : Skjuter massa acid klot över spel planen som varar på plats en stud.
		eStabSweep, // 2 : Gör en stor sweep över planen.
		eStabAndStuck, // 3 : Gör ett antal stab attacker och sedan anfaller med huvudet och fastnar.
		ePlzHelp // 4 : kallar fiender för hjälp.
	};

	enum class BossAnimations
	{
		eIdle,
		eDeath,
		eRoar,
		eAcidSpit,
		eStabSweep,
		eStabs,
		eHeadbang,
		eHeadStuckReverse,
		eStuck,
		eSad
	};

	class BossBobBehaviour : public Behaviour
	{
	public:

		void Init() override;
		void Update(float aDeltaTime) override;

		void SetIsActive(bool aStatement) { myIsActive = aStatement; }
		bool GetIsActive() const { return myIsActive; }
		void UpdateWithDatabase(CU::Database<true>& aBehaviourDatabase) override;

		void OnTrigger(EntityID) override;

		const float GetHealth();

	private:
		friend class BossTrigger;
		friend class State_HUD;

		void CheckStage();
		void SwitchStage(int aNewStage);
		void LoadStageData();
		void SetAllSpawnPools(std::vector<CU::Vec3f> aPool);
		void SetSpawnPool(std::vector<CU::Vec3f> aPool, int aStage);

		void Rotate(CU::Vec3f aPos, float aLerpValue = 1.f);
		void DamageShader();
		void OnIFrame();

		void StartIdleTimer();
		void GetNewStateOrder();
		void DoStateOrder(float aDeltaTime);
		void DoRoarEvent();
		/*---------------Acid Spit Functions-----------------------------------*/
		void DoAcidSpit(float aDeltaTime);
		void ResetAcidSpit();
		void ResetAcidSpit(int aStage);
		void CreatePuddle(CU::Vec3f aTarget, int aStage);
		CU::Vec3f TryAcidSpit();
		/*---------------Stab Sweep Functions----------------------------------*/
		void DoStabSweep(float aDeltaTime);
		void ResetStabSweep();
		void ResetStabSweep(int aStage);
		void TryStabSweep();
		/*---------------Stab And Stuck Functions------------------------------*/
		void DoStabAndStuck(float aDeltaTime);
		void ResetStabAndStuck();
		void ResetStabAndStuck(int aStage);
		/*---------------Stab Functions----------------------------------------*/
		void PreStab();
		void Stab(CU::Vec3f aTarget, int aStage);
		CU::Vec3f TryStab();
		/*---------------Head Attack Functions---------------------------------*/
		void PreHead();
		void UseHead(CU::Vec3f aTarget, int aStage);
		/*---------------Roar Help Functions-----------------------------------*/
		void DoRoarHelp(float aDeltaTime);
		void ResetRoarHelp();
		void ResetRoarHelp(int aStage);
		void SpawnLurker(int aStage);
		void SpawnHunter(int aStage);

		bool myIsActive;
		bool myIsIdle;
		bool myOnIFrameMode;
		bool myUnderOrderDelay;
		bool myDoingAnAttack;

		float myDamageShaderDuration;
		float myRotationSpeed;

		BossState myState = BossState::eNotInFight;

		Scene* myActiveScene = nullptr;
		TransformComponent* myTransform = nullptr;
		TransformComponent* myPlayerTransform = nullptr;
		EnemyStatsComponent* myStats = nullptr;

		// Saved structs
		std::vector<StabAndStuck> myStabAndStuckData; // Saved data for each stage for this state.
		std::vector<StabSweep> myStabSweepData; // Saved data for each stage for this state.
		std::vector<AcidSpit> myAcidSpitData; // Saved data for each stage for this state.
		std::vector<RoarHelp> myRoarHelpData; // Saved data for each stage for this state.

		std::vector<std::vector<int>> myAttackOptions; // ints for enum.
		std::vector<float> myOrderDelays; // Delays on attack options switch, depending on stage.

		std::unordered_map<std::string, EntityID> myAttackIds;

		CU::Vec3f myPlayerLastSeenPos;

		int myStage; // What stage the boss is in depending on health
		
		unsigned int myAttackInt;

		// Animation stuff
		AnimationHelper<BossAnimations> myAnimationHelper;
		// All Ids
		AnimationID myIdleAnim;
		AnimationID myDeathAnim;
		AnimationID myRoarAnim;
		AnimationID myAcidSpitAnim;
		AnimationID myStabSweepAnim;
		AnimationID myStabsAnim;
		AnimationID myHeadbangAnim;
		AnimationID myHeadStuckReverseAnim;
		AnimationID myStuckAnim;
		AnimationID mySadAnim;
	};
}