
#include "State_Play.h"

#include "ISTE/Context.h"

#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Physics/PhysicsEngine.h"

#include "ISTE/CU/InputHandler.h"
#include "ISTE/StateStack/StateManager.h"
#include "ISTE/StateStack/StateType.h"

#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/ComponentsAndSystems/LoDSystem.h"

#include "ISTE/Audio/AudioHandler.h"
#include "ISTE/Audio/AudioSourceSystem.h"

#include "ISTE/UI/UIHandler.h"

ISTE::State_Play::~State_Play()
{}

void ISTE::State_Play::Init()
{
	SetStateFlags(StateFlags_PassthruUpdate | StateFlags_PassthruRender);

	const int sceneIndex = Context::Get()->mySceneHandler->GetActiveSceneIndex();

	Context::Get()->myAudioHandler->StopAllPreLoadedSounds();

	switch (sceneIndex)
	{
	case 1:
	case 2:
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eMusic_OldTown);
		break;
	case 3:
	case 4:
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eMusic_Catacombs);
		break;
	case 5:
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eMuisc_Menu);
		break;
	case 6:
		Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eMusic_Boss);
		break;
	default:
		Context::Get()->myAudioHandler->StopAllPreLoadedSounds();
	}

	myElapsedTime = 0.f;

	myCtx->myUIHandler->EnableMouseEmitter(false);
}

void ISTE::State_Play::Update(const float aDeltaTime)
{
	myElapsedTime += aDeltaTime;

	myCtx->mySystemManager->GetSystem<AudioSourceSystem>()->PauseAll(SoundTypes::eSFX, false); //this is very ugly

	if (myCtx->myInputHandler->IsKeyDown(VK_ESCAPE))
		myCtx->myStateManager->PushState(ISTE::StateType::MenuPause);

	myCtx->mySystemManager->GetSystem<LoDSystem>()->UpdateAgainstPlayer();

	myCtx->mySceneHandler->Update(aDeltaTime);
	myCtx->myPhysicsEngine->Update();
}

void ISTE::State_Play::Render()
{
}
