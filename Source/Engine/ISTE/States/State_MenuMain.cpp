
#include "State_MenuMain.h"

#include "ISTE/Context.h"

#include "ISTE/UI/UIHandler.h"
#include "ISTE/UI/UIWorker.h"

#include "ISTE/Audio/AudioHandler.h"

#include "ISTE/Scene/Scene.h"
#include "ISTE/Scene/SceneHandler.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/Graphics/ComponentAndSystem/TransformComponent.h"
#include "ISTE/CU/ReadWrite.h"
#include "ISTE/WindowsWindow.h"
#include "ISTE/ECSB/PlayerBehaviour.h"

ISTE::State_MenuMain::~State_MenuMain()
{}

void ISTE::State_MenuMain::Init()
{
	SetStateFlags(StateFlags_AlwaysRender);

	myElapsedTime = 0.f;

	if (myCtx->myUIHandler->myUIDatas.count("MainMenu"))
		myUI = &myCtx->myUIHandler->myUIDatas.at("MainMenu");

	Context::Get()->myAudioHandler->StopAllPreLoadedSounds();
	Context::Get()->myAudioHandler->PlayPreLoadedSound(ISTE::PreLoadedSounds::eMuisc_Menu);

	myCtx->myUIHandler->EnableMouseEmitter(true);

	LoadLevelAndSetCamera();
}

void ISTE::State_MenuMain::Update(const float aDeltaTime)
{
	myElapsedTime += aDeltaTime;

	if (myUI != nullptr)
		ProcessUI(*myUI);

	Scene& scene = myCtx->mySceneHandler->GetActiveScene();
	PlayerBehaviour* pB = scene.GetComponent<PlayerBehaviour>(scene.GetPlayerId());
	if (pB != nullptr)
		pB->UpdateWeapon();
}

void ISTE::State_MenuMain::Render()
{
	if (myUI != nullptr)
		AddRenderCommands(*myUI);
}

void ISTE::State_MenuMain::LoadLevelAndSetCamera()
{
	auto sh = myCtx->mySceneHandler;
	sh->LoadScene(0, true);

	Scene& scene = sh->GetActiveScene();
	PlayerBehaviour* pB = scene.GetComponent<PlayerBehaviour>(scene.GetPlayerId());

	myCtx->myGraphicsEngine->SetCamera(pB->GetCamera());

}