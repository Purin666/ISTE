#include "EndLevelTrigger.h"

#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/ECSB/PlayerBehaviour.h"

#include "ISTE/StateStack/StateManager.h"
#include "ISTE/StateStack/StateType.h"
#include "ISTE/CU/Database.h"

namespace ISTE
{
	void EndLevelTriggerBehaviour::OnTrigger(EntityID aId)
	{
		SceneHandler* handler = Context::Get()->mySceneHandler;

		//if (handler->GetActiveScene().GetComponent<PlayerBehaviour>(aId))
		if(handler->GetActiveScene().GetPlayerId() == aId)
		{
			int buildSize = handler->GetTotalBuildScenes();
			int activeScene = handler->GetActiveSceneIndex();

			auto genericDatabase = Context::Get()->myGenericDatabase;
			auto stateManager = Context::Get()->myStateManager;

			if ((activeScene + 1) == buildSize)
			{
				// last level completed
				genericDatabase->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "MainMenu");
				genericDatabase->SetValue<size_t>("SceneToLoadAfterFadeOut", 0);
				stateManager->PushState(ISTE::StateType::FadeOut);
				//handler->LoadScene(0);
			}
			else
			{
				// load next level
				genericDatabase->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "Play");
				genericDatabase->SetValue<size_t>("SceneToLoadAfterFadeOut", activeScene + 1);
				stateManager->PushState(ISTE::StateType::FadeOut);
				//handler->LoadScene(activeScene + 1);
			}


		}
	}
}
