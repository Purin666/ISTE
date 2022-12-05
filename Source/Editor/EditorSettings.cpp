#include "EditorSettings.h"

#include "EditorContext.h"
#include "imgui/imgui.h"

#include "imgui/misc/cpp/imgui_stdlib.h"

EditorSettings::EditorSettings()
{
}

void EditorSettings::Draw()
{

	if (!myIsActive)
		return;

	ImGui::Begin("EditorSettings", &myIsActive);

	ImGui::ShowStyleSelector("Style");

	if (ImGui::Button("Edit Style"))
		myCreateStyle = true;


	CustomStyle();

	ImGui::End();
}

void EditorSettings::CustomStyle()
{
	if (!myCreateStyle)
		return;

	ImGui::Begin("StyleEditor", &myCreateStyle);

	ImGui::ShowStyleEditor();

	ImGui::End();

}
