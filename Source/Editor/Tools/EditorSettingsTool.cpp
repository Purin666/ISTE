#include "EditorSettingsTool.h"
#include "BaseEditor.h"
#include "imgui/imgui.h"

void ISTE::EditorSettingsTool::Init(BaseEditor* anEditor)
{
	myTools = anEditor->GetTools();
	myToolName = "Tool Settings";
}

void ISTE::EditorSettingsTool::Draw()
{
	if (!myActive)
		return;
	ImGui::Begin(myToolName.c_str(), &myActive);
	for (AbstractTool* i : myTools)
	{
		i->Settings();
	}
	ImGui::End();
}