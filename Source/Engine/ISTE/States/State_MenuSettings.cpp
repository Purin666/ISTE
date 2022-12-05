
#include "State_MenuSettings.h"

#include "ISTE/Context.h"

#include "ISTE/UI/UIHandler.h"
#include "ISTE/UI/UIWorker.h"

#include "ISTE/CU/InputHandler.h"
#include "ISTE/StateStack/StateManager.h"

ISTE::State_MenuSettings::~State_MenuSettings()
{}

void ISTE::State_MenuSettings::Init()
{
	SetStateFlags(StateFlags_PassthruRender);

	myElapsedTime = 0.f;

	if (myCtx->myUIHandler->myUIDatas.count("Settings"))
		myUI = &myCtx->myUIHandler->myUIDatas.at("Settings");
}

void ISTE::State_MenuSettings::Update(const float aDeltaTime)
{
	myElapsedTime += aDeltaTime;

	if (myCtx->myInputHandler->IsKeyDown(VK_ESCAPE))
		myCtx->myStateManager->PopState();

	if (myUI != nullptr)
		ProcessUI(*myUI);
}

void ISTE::State_MenuSettings::Render()
{
	if (myUI != nullptr)
		AddRenderCommands(*myUI);
}
