#pragma once

#include "ISTE/ECSB/behaviour.h"

#include "ISTE/ECSB/ECSDefines.hpp"
#include "ISTE/Math/Vec3.h"

#include <vector>

namespace ISTE
{
	class Scene;
	class BossTrigger : public Behaviour
	{
	public:

		void Init() override;

		void Update(float aDeltaTime) override;

	private:
		friend class EventTriggerBuilder;

		int mySpawnPoolSize;

		bool myIsBossActive;
		bool myUnderTransition;
		bool myUseCamera;
		bool myUseRoar;

		ISTE::Scene* myScene;

		float myRadius;
		float myPanTimer;
		float myWaitTimer;
		float myDegreesToRotate;
		float myShakeLerpTimer = 0;
		float myRoarBlurTimer = 0;
		float myPostRoarWaitTimer = 0;
		float myPostRoarWaitDuration = 0.85;
		std::vector<CU::Vec3f> mySpawnPool;
	};
}