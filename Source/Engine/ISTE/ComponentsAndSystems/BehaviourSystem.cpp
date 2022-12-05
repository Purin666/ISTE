#include "BehaviourSystem.h"

#include "../Context.h"
#include "../Scene/SceneHandler.h"
#include "../Scene/Scene.h"
#include "../ECSB/Behaviour.h"


namespace ISTE
{
	void BehaviourSystem::Init()
	{
		Scene& scene = mySceneHandler->GetActiveScene();
		scene.LoadBehaviours();
		for (auto& id : myEntities[0])
		{
			BehaviourHandle& handle = scene.GetBehaviourHandle(id);

			for (auto& b : handle.GetEverything())
			{
				b->Init();
			}

		}
		for (auto& id : myEntities[0])
		{
			BehaviourHandle& handle = scene.GetBehaviourHandle(id);

			for (auto& b : handle.GetEverything())
			{
				b->UpdateWithDatabase(scene.GetBehaviourDatabase());
			}

		}
	}
	void BehaviourSystem::Update(float aTimeDelta)
	{
		Scene& scene = mySceneHandler->GetActiveScene();
		if (scene.GetUpdateBehaviours())
		{
			for (auto& id : myEntities[0])
			{
				BehaviourHandle& handle = scene.GetBehaviourHandle(id);

				for (auto& b : handle.GetEverything())
				{
					b->UpdateWithDatabase(scene.GetBehaviourDatabase());
				}

			}
			scene.GetUpdateBehaviours() = false;
		}
		for (auto& id : myEntities[0])
		{
			BehaviourHandle& handle = scene.GetBehaviourHandle(id);

			for (auto& b : handle.GetEverything())
			{
				b->Update(aTimeDelta);
			}

		}
	}
}
