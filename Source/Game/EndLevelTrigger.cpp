#include "EndLevelTrigger.h"

#include "ISTE/Context.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/ECSB/PlayerBehaviour.h"

namespace ISTE
{
	void EndLevelTriggerBehaviour::OnTrigger(EntityID aId)
	{
		SceneHandler* handler = Context::Get()->mySceneHandler;

		if (handler->GetActiveScene().GetComponent<PlayerBehaviour>(aId))
		{
			int buildSize = handler->GetTotalBuildScenes();
			int activeScene = handler->GetActiveSceneIndex();

			if ((activeScene + 1) == buildSize)
			{
				handler->LoadScene(0);
			}
			else
			{
				handler->LoadScene(activeScene + 1);
			}
		}
	}
}
