
#include "State_Lose.h"

#include "ISTE/Context.h"

#include "ISTE/UI/UIHandler.h"
#include "ISTE/UI/UIWorker.h"

#include "ISTE/StateStack/StateManager.h"
#include "ISTE/StateStack/StateType.h"
#include "ISTE/Audio/AudioHandler.h"
#include "ISTE/CU/Database.h"
#include "ISTE/CU/InputHandler.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/VFX/SpriteParticles/Sprite2DParticleHandler.h"
#include "ISTE/CU/Database.h"

ISTE::State_Lose::~State_Lose()
{

}

void ISTE::State_Lose::Init()
{
	SetStateFlags(StateFlags_AlwaysRender);

	myDuration = 3.f;

	auto& datas = myCtx->myUIHandler->myUIDatas;

	if (datas.count("LoseScreen"))
		myUI = &datas.at("LoseScreen");


	//Context::Get()->myAudioHandler->StopAllPreLoadedSounds();
	//Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eMuisc_MenuNBoss);
}

void ISTE::State_Lose::Update(const float aDeltaTime)
{
	myElapsedTime += aDeltaTime;

	if (myUI != nullptr)
		ProcessUI(*myUI);

	const bool skip = myCtx->myInputHandler->IsKeyDown(VK_RETURN) ||
					  myCtx->myInputHandler->IsKeyDown(VK_SPACE);

	if (myDuration < myElapsedTime || skip)
	{
		auto ph = myCtx->mySprite2DParticleHandler;
		auto db = myCtx->myGenericDatabase;
		ph->DeactivateEmitter(db->Get<int>("DeathScreen_Emitter_1"));
		ph->DeactivateEmitter(db->Get<int>("DeathScreen_Emitter_2"));
		ph->DeactivateEmitter(db->Get<int>("DeathScreen_Emitter_3"));
		ph->DeactivateEmitter(db->Get<int>("DeathScreen_Emitter_4"));

		size_t currentScene = myCtx->mySceneHandler->GetActiveSceneIndex();

		db->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "Play");
		db->SetValue<size_t>("SceneToLoadAfterFadeOut", currentScene);
		db->SetValue("ReloadScene", true);
		myCtx->myStateManager->PushState(ISTE::StateType::FadeOut);

	}
}

void ISTE::State_Lose::Render()
{
	if (myUI != nullptr)
		AddRenderCommands(*myUI);
}