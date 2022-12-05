#pragma once
#include "Behaviour.h"
#include "ISTE/Math/Vec3.h"

#include <ISTE/Graphics/RenderDefines.h>

#include <queue>
#include <map>

#include "ISTE/Graphics/Helper/AnimationHelper.h"

enum class HunterAnimations
{
	eIdle,
	eDead,
	eAttack,
	eMovement,
	ePreExplosion,
	eExplode
};

//offset cause of shared sounds
enum class HunterSounds
{
	eAttack = 1,
	ePreExplosion = 2,
	eExplosion = 3
};

namespace ISTE
{
	class Scene;
	class AudioSource;
	struct TransformComponent;
	struct EnemyStatsComponent;

	class HunterEnemyBehaviour : public Behaviour
	{
	public:

		~HunterEnemyBehaviour();

		void Init() override;
		void Update(float aDeltaTime) override;
		void OnTrigger(EntityID aId) override;

		void SetIsActive(bool aStatement) { myIsActive = aStatement; }
		bool GetIsActive() const { return myIsActive; }
		void UpdateWithDatabase(CU::Database<true>& aBehaviourDatabase) override;

		void Reset();
		
	private:
		friend class EnemyBuilder;
		friend class BossBobBehaviour;

		bool CanSeePlayer();
		void HuntMode(float aDeltaTime);
		void BuildPath();
		void Attack(float aDeltaTime);
		void PreExplosion();
		void Explosion();

		int myExperience = 0;

		bool myIsActive;
		bool myHasAnOrder;
		bool myHasIdleBehaviour;
		bool myCanAttack;
		bool myWarningPlayer;
		bool myFrameWait = false;
		bool myWaitForAnim = false;
		bool myWaitForExplosion = false;
		bool myWaitForReAllignment = false;
		bool myIsElite = false;
		bool myIsStaled = false;


		int myProjectileCount;

		float mySpeed;
		float myFindRadius;
		float myTimer;
		float myMaxTimer;
		float myKeepDistance;

		float mySuicideDistance;
		float myWarningDelay;
		float myExplosionRange;
		float myExplosionDamage;

		float myStalTime = 0.1f;
		float myStalTimer = 0;

		std::map<std::string, EntityID> myProjectileIds;

		Scene* myActiveScene = nullptr;
		TransformComponent* myTransform = nullptr;
		EnemyStatsComponent* myStats = nullptr;
		AudioSource* myAudioSource = nullptr;

		EntityID myCircleVFX = INVALID_ENTITY;
		EntityID myExplosionId = INVALID_ENTITY;

		std::queue<CU::Vec3f> myOrders;

		CU::Vec3f myPlayerLastSeenPos;
		CU::Vec3f myLastToPoint;

		ModelID myExplodeModel;

		AnimationHelper<HunterAnimations> myAnimationHelper;
		//temp handle
		AnimationID myIdleAnim;
		AnimationID myDeadAnim;
		AnimationID myAttackAnim;
		AnimationID myMovmentAnim;
		AnimationID myPreExplodeAnim;
		AnimationID myExplodeAnim;


		//

	};
}