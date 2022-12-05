#include "State_Win.h"

#include "ISTE/Context.h"

#include "ISTE/UI/UIHandler.h"
#include "ISTE/UI/UIWorker.h"

#include "ISTE/StateStack/StateManager.h"
#include "ISTE/StateStack/StateType.h"
#include "ISTE/Audio/AudioHandler.h"
#include "ISTE/CU/Database.h"
#include "ISTE/CU/InputHandler.h"

void ISTE::State_Win::Init()
{
	SetStateFlags(StateFlags_AlwaysRender);

	myDuration = 3.f;

	auto& datas = myCtx->myUIHandler->myUIDatas;

	if (datas.count("WinScreen"))
		myUI = &datas.at("WinScreen");

	//Context::Get()->myAudioHandler->StopAllPreLoadedSounds();
	//Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eMuisc_MenuNBoss);
}

void ISTE::State_Win::Update(const float aDeltaTime)
{
	myElapsedTime += aDeltaTime;

	if (myUI != nullptr)
		ProcessUI(*myUI);

	const bool skip = myCtx->myInputHandler->IsKeyDown(VK_RETURN) ||
					  myCtx->myInputHandler->IsKeyDown(VK_SPACE);

	if (myDuration < myElapsedTime || skip)
	{
		auto db = myCtx->myGenericDatabase;
		auto sm = myCtx->myStateManager;

		db->SetValue<std::string>("StackOfStatesToLoadAfterFadeOut", "MainMenu");
		db->SetValue<size_t>("SceneToLoadAfterFadeOut", 0);
		sm->PushState(ISTE::StateType::FadeOut);
	}
}

void ISTE::State_Win::Render()
{
	if (myUI != nullptr)
		AddRenderCommands(*myUI);
}