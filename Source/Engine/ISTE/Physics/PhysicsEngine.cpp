#include "PhysicsEngine.h"

#include "ISTE/Context.h"
#include "ISTE/ECSB/SystemManager.h"

#include "ISTE/Physics/ComponentsAndSystems/TriggerSystem.h"

namespace ISTE
{
	void PhysicsEngine::Init()
	{
		myTriggerSystem = Context::Get()->mySystemManager->GetSystem<TriggerSystem>();
	}

	void PhysicsEngine::Update()
	{
		myTriggerSystem->Check();
	}
}
