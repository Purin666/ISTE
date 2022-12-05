#include "GameRegister.h"
#include "ISTE/Context.h"
#include "ISTE/Scene/Scene.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/ECSB/PlayerBehaviour.h"

// Enemy Behaviours
#include "ISTE/ECSB/IdleEnemyBehaviour.h"
#include "ISTE/ECSB/LurkerEnemyBehaviour.h"
#include "ISTE/ECSB/HunterEnemyBehaviour.h"
#include "ISTE/ECSB/BossBobBehaviour.h"

// Triggers
#include "ISTE/Physics/EndLevelTrigger.h"
#include "ISTE/Text/TextTrigger.h"
#include "ISTE/ComponentsAndSystems/BossTrigger.h"

#include "ISTE/CU/MemTrack.hpp"

namespace ISTE
{

	void GameRegister::Init()
	{
		myCtx = Context::Get();
		mySceneHandler = myCtx->mySceneHandler;
		mySystemManager = myCtx->mySystemManager;

		//RegisterComponents();
		//RegisterBehaviours();
		//RegisterSystems();

	}
	void GameRegister::RegisterComponents()
	{
	}
	void GameRegister::RegisterBehaviours()
	{
		mySceneHandler->RegisterBehaviour<ISTE::PlayerBehaviour>();
		mySceneHandler->RegisterBehaviour<ISTE::IdleEnemyBehaviour>();
		mySceneHandler->RegisterBehaviour<ISTE::LurkerEnemyBehaviour>();
		mySceneHandler->RegisterBehaviour<ISTE::HunterEnemyBehaviour>();
		mySceneHandler->RegisterBehaviour<ISTE::BossBobBehaviour>();
		mySceneHandler->RegisterBehaviour<ISTE::EndLevelTriggerBehaviour>();
		mySceneHandler->RegisterBehaviour<ISTE::TextTrigger>();
		mySceneHandler->RegisterBehaviour<ISTE::BossTrigger>();
	}
	void GameRegister::RegisterSystems()
	{
	}
}