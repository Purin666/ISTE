#pragma once


#include "ISTE/ECSB/System.h"
#include "ISTE/CU/RayFromScreen.h"

namespace ISTE
{

	class EnemyBehaviourSystem : public System
	{
	public:
		~EnemyBehaviourSystem() = default;
		void Init();

		EntityID GetEntityIDFromScreen();
		void UpdateHealthBarDrawCalls();

		inline std::set<EntityID> GetEntitySet() { return myEntities[0]; }

		void KillAll();

	private:
		TextureID myHeltBarTexture;
		ScreenHelper myHelper;
	};
}