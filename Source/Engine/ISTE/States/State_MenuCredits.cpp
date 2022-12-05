
#include "State_MenuCredits.h"

#include "ISTE/Context.h"

#include "ISTE/UI/UIHandler.h"
#include "ISTE/UI/UIWorker.h"

#include "ISTE/CU/InputHandler.h"
#include "ISTE/StateStack/StateManager.h"

ISTE::State_MenuCredits::~State_MenuCredits()
{}

void ISTE::State_MenuCredits::Init()
{
	SetStateFlags(StateFlags_PassthruRender);

	myElapsedTime = 0.f;

	if (myCtx->myUIHandler->myUIDatas.count("Credits"))
		myUI = &myCtx->myUIHandler->myUIDatas.at("Credits");
}

void ISTE::State_MenuCredits::Update(const float aDeltaTime)
{
	myElapsedTime += aDeltaTime;

	if (myCtx->myInputHandler->IsKeyDown(VK_ESCAPE))
		myCtx->myStateManager->PopState();

	if (myUI != nullptr)
		ProcessUI(*myUI);
}

void ISTE::State_MenuCredits::Render()
{
	if (myUI != nullptr)
		AddRenderCommands(*myUI);
}
