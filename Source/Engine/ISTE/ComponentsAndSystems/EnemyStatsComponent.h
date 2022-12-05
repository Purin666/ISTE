#pragma once

#include <string>

namespace ISTE
{
	struct EnemyStatsComponent
	{
		EnemyStatsComponent(EnemyStatsComponent& aLeft) {
			memcpy(this, &aLeft, sizeof(EnemyStatsComponent));
		};
		EnemyStatsComponent() {};
		~EnemyStatsComponent() {};
		EnemyStatsComponent& operator= (EnemyStatsComponent& other)
		{
			memcpy(this, &other, sizeof(EnemyStatsComponent));
			return *this;
		}

		float myHealth = 0.f;
		float myMaxHealth = 0.f;

		// Weapon
		float myDamage = 0.f;
		float myAttackSize = 0.f;
		float myAttackSpeed = 0.f;
		float myStayRate = 0.f;
		float mySpeed = 0.f;
		float myMiniumDistance = 0.f;

		bool myIsDead = false;
		bool myIsElite = false;

		std::string myName = "Default";
	};
}