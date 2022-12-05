#pragma once

#include "../ECSB/System.h"

namespace ISTE
{
	class SceneHandler;

	class BehaviourSystem : public System
	{
	public:
		void Init();
		void Update(float aTimeDelta);

	private:
		friend class SceneHandler;

		SceneHandler* mySceneHandler;
	};
}