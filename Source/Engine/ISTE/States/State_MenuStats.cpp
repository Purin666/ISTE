
#include "State_MenuStats.h"

#include "ISTE/Context.h"

#include "ISTE/UI/UIHandler.h"
#include "ISTE/UI/UIWorker.h"

#include "ISTE/CU/InputHandler.h"
#include "ISTE/StateStack/StateManager.h"

ISTE::State_MenuStats::~State_MenuStats()
{}

void ISTE::State_MenuStats::Init()
{
	SetStateFlags(StateFlags_PassthruRender);

	myElapsedTime = 0.f;

	if (myCtx->myUIHandler->myUIDatas.count("Stats"))
		myUI = &myCtx->myUIHandler->myUIDatas.at("Stats");
}

void ISTE::State_MenuStats::Update(const float aDeltaTime)
{
	myElapsedTime += aDeltaTime;

	if (myCtx->myInputHandler->IsKeyDown(VK_ESCAPE))
		myCtx->myStateManager->PopState();

	if (myUI != nullptr)
		ProcessUI(*myUI);
}

void ISTE::State_MenuStats::Render()
{
	if (myUI != nullptr)
		AddRenderCommands(*myUI);
}
