
#include "State_MenuLevelSelect.h"

#include "ISTE/Context.h"

#include "ISTE/UI/UIHandler.h"
#include "ISTE/UI/UIWorker.h"

#include "ISTE/CU/InputHandler.h"
#include "ISTE/StateStack/StateManager.h"

ISTE::State_MenuLevelSelect::~State_MenuLevelSelect()
{}

void ISTE::State_MenuLevelSelect::Init()
{
	SetStateFlags(StateFlags_PassthruRender);

	myElapsedTime = 0.f;

	if (myCtx->myUIHandler->myUIDatas.count("LevelSelect"))
		myUI = &myCtx->myUIHandler->myUIDatas.at("LevelSelect");
}

void ISTE::State_MenuLevelSelect::Update(const float aDeltaTime)
{
	myElapsedTime += aDeltaTime;

	if (myCtx->myInputHandler->IsKeyDown(VK_ESCAPE))
		myCtx->myStateManager->PopState();

	if (myUI != nullptr)
		ProcessUI(*myUI);
}

void ISTE::State_MenuLevelSelect::Render()
{
	if (myUI != nullptr)
		AddRenderCommands(*myUI);
}
