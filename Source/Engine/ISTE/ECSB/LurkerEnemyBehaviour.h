#pragma once
#include "Behaviour.h"
#include "ISTE/Math/Vec3.h"

#include <ISTE/Graphics/RenderDefines.h>

#include <queue>

#include "ISTE/Graphics/Helper/AnimationHelper.h"

#include "ISTE/Time/LocalCountDown.h"

enum class LurkerAnimations
{
	eIdle, 
	eMovement,
	eDead,
	eAttack
};

//offset cause of shared sounds
enum class LurkerSounds
{
	eAttack = 1,
	eDeath = 2,
};

namespace ISTE
{
	class Scene;
	struct TransformComponent;
	struct EnemyStatsComponent;
	class AudioSource;

	class LurkerEnemyBehaviour : public Behaviour
	{
	public:
		~LurkerEnemyBehaviour();
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
		void LurkMode(float aDeltaTime);
		void BuildPath();
		void Attack();

		int myExperience = 0;

		bool myIsActive;
		bool myHasAnOrder;
		bool myHasIdleBehaviour;
		bool myCanAttack;
		bool myWaitForAnim = false;
		bool myWaitForReAllignment = false;
		bool myIsElite = false;
		bool myHaveGivenExperience = false;

		bool myFirstAttackDelay = true;
		bool myTimerRunning = false;

		float mySpeed;
		float myFindRadius;
		float myTimer;
		float myMaxTimer;
		float myKeepDistance;
		float myAnimDelay = 0.5f;
		float myFirstAttackTimerDuration = 0.25f;

		EntityID myAttackId;

		Scene* myActiveScene = nullptr;
		TransformComponent* myTransform = nullptr;
		EnemyStatsComponent* myStats = nullptr;
		AudioSource* myAudioSource = nullptr;

		std::queue<CU::Vec3f> myOrders;

		CU::Vec3f myPlayerLastSeenPos;
		CU::Vec3f myLastToPoint;

		AnimationHelper<LurkerAnimations> myAnimationHelper;

		ISTE::LocalCountDown myFirstAttackDelayCountDown;

		//temp handle
		AnimationID myIdleAnim;
		AnimationID myDeadAnim;
		AnimationID myAttackAnim;
		AnimationID myMovmentAnim;

		//
	};
}