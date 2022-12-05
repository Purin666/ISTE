
#include "State_MenuPause.h"

#include "ISTE/Context.h"

#include "ISTE/UI/UIHandler.h"
#include "ISTE/UI/UIWorker.h"

#include "ISTE/CU/InputHandler.h"
#include "ISTE/StateStack/StateManager.h"

#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Audio/AudioSourceSystem.h"

#include "ISTE/Time/TimeHandler.h"
#include "ISTE/UI/UIHandler.h"

#include "ISTE/VFX/VFXHandler.h"
#include "ISTE/VFX/ModelVFX/ModelVFXHandler.h"
#include "ISTE/VFX/SpriteParticles/Sprite3DParticleHandler.h"
#include "ISTE/Graphics/ComponentAndSystem/AnimationDrawerSystem.h"

ISTE::State_MenuPause::~State_MenuPause()
{
	myCtx->myTimeHandler->ResumeAllPausedTimers();
	
	myCtx->myVFXHandler->SetPaused(false);
	myCtx->myModelVFXHandler->SetPaused(false);
	myCtx->mySprite3DParticleHandler->SetPaused(false);

	AnimationDrawerSystem* ADS = myCtx->mySystemManager->GetSystem<AnimationDrawerSystem>();
	ADS->ResumeAnimations();

	myCtx->myUIHandler->EnableMouseEmitter(false);
}

void ISTE::State_MenuPause::Init()
{
	SetStateFlags(StateFlags_PassthruRender);

	myElapsedTime = 0.f;

	if (myCtx->myUIHandler->myUIDatas.count("Pause"))
		myUI = &myCtx->myUIHandler->myUIDatas.at("Pause");

	myCtx->mySystemManager->GetSystem<AudioSourceSystem>()->PauseAll(SoundTypes::eSFX, true);

	myCtx->myTimeHandler->PauseAllActiveTimers();

	myCtx->myVFXHandler->SetPaused(true);
	myCtx->myModelVFXHandler->SetPaused(true);
	myCtx->mySprite3DParticleHandler->SetPaused(true);

	AnimationDrawerSystem* ADS = myCtx->mySystemManager->GetSystem<AnimationDrawerSystem>();
	ADS->PauseAnimations();

	myCtx->myUIHandler->EnableMouseEmitter(true);
}

void ISTE::State_MenuPause::Update(const float aDeltaTime)
{
	myElapsedTime += aDeltaTime;

	if (myCtx->myInputHandler->IsKeyDown(VK_ESCAPE))
	{
		myCtx->myStateManager->PopState();
	}

	if (myUI != nullptr)
		ProcessUI(*myUI);
}

void ISTE::State_MenuPause::Render()
{
	if (myUI != nullptr)
		AddRenderCommands(*myUI);
}
