#include "StateTool.h"

#include "imgui/imgui.h"

#include "ISTE/Context.h"
#include "ISTE/StateStack/StateManager.h"
#include "ISTE/StateStack/StateType.h"

void ISTE::StateTool::Init(BaseEditor*)
{
	myCtx = Context::Get();
	myToolName = "State Tool";
}

void ISTE::StateTool::Draw()
{
	if (!myActive)
		return;

	ImGui::Begin(myToolName.data(), &myActive);

	if (ImGui::Button("Push Splash Screen"))
	{
		myCtx->myStateManager->LoadStack("Splash Screen");
	}
	if (ImGui::Button("Push Main Menu"))
	{
		myCtx->myStateManager->PushState(StateType::MenuMain);
	}
	if (ImGui::Button("Pop All States"))
	{
		myCtx->myStateManager->PopAllStates();
	}

	ImGui::End();
}
