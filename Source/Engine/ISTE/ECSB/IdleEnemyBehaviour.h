#pragma once
#include "Behaviour.h"
#include "ISTE/Math/Vec3.h"

#include <random>
#include <queue>

namespace ISTE
{
	class Scene;
	struct TransformComponent;
	struct EnemyStatsComponent;

	enum class SharedSounds
	{
		eTookDamage = 0
	};

	class IdleEnemyBehaviour : public Behaviour
	{
	public:

		void Init() override;
		void Update(float aDeltaTime) override;

		void Reset();

		void SetIsActive(bool aStatement) { myIsActive = aStatement; }
		bool GetIsActive() const { return myIsActive; }

		void OnTrigger(EntityID) override;

		bool Reseting() { return myOnRest; }

	private:

		void MoveToIdle(float aDeltaTime);
		void BuildPath();
		void DamageShader();

		bool myIsActive;
		bool myHasAnOrder;
		bool myOnRest;
		bool myOnIFrameMode;

		float myRadius;
		float mySpeed;
		float myTimer;
		float myMaxTimer;
		float myDamageShaderDuration;

		std::uniform_real_distribution<float> myRandomFloat;

		std::mt19937 myRandomEngine;

		Scene* myActiveScene = nullptr;
		TransformComponent* myTransform = nullptr;
		EnemyStatsComponent* myStats = nullptr;

		std::queue<CU::Vec3f> myOrders;

		CU::Vec3f myOrgin;

	};
}