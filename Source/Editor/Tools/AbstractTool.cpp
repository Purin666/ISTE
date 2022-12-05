#include "AbstractTool.h"
#include "imgui/imgui.h"
void ISTE::AbstractTool::CheckActive()
{
	ImGui::MenuItem(myToolName.c_str(), nullptr, &myActive);
}
void ISTE::AbstractTool::Settings()
{
}
