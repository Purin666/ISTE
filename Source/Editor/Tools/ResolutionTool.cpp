#include "ResolutionTool.h"
#include "imgui/imgui.h"
#include "ISTE/Context.h"
#include "ISTE/WindowsWindow.h"
#include "ISTE/Math/Vec.h"
#include "BaseEditor.h"

void ISTE::ResolutionTool::Init(BaseEditor*)
{
	myToolName = "Resolution";
}

void ISTE::ResolutionTool::Draw()
{
	if (!myActive) return;

	ImGui::Begin("Resolution", &myActive);

	Context* ctx = ISTE::Context::Get();
	WindowsWindow* wnd = ctx->myWindow;
	auto imguiVP = ImGui::GetWindowViewport();

	if (ImGui::Selectable("Fullscreen"))
	{
		wnd->ToggleFullscreen();
		CU::Vec2Ui res = wnd->GetResolution();
		imguiVP->Size = ImVec2(res.x, res.y);
		ISTE::BaseEditor::myWantToUpdateSize = true;
	}

	if (ImGui::Selectable("1440p"))
	{
		wnd->SetResolution(CU::Vec2Ui(2560, 1440));
		ISTE::BaseEditor::myWantToUpdateSize = true;
	}
	if (ImGui::Selectable("1080p"))
	{
		wnd->SetResolution(CU::Vec2Ui(1920, 1080));
		ISTE::BaseEditor::myWantToUpdateSize = true;
	}
	if (ImGui::Selectable("900p"))
	{
		wnd->SetResolution(CU::Vec2Ui(1600, 900));
		ISTE::BaseEditor::myWantToUpdateSize = true;
	}
	if (ImGui::Selectable("720p"))
	{
		wnd->SetResolution(CU::Vec2Ui(1280, 720));
		ISTE::BaseEditor::myWantToUpdateSize = true;
	}
	if (ImGui::Selectable("540p"))
	{
		wnd->SetResolution(CU::Vec2Ui(960, 540));
		ISTE::BaseEditor::myWantToUpdateSize = true;
	}
	if (ImGui::Selectable("360p"))
	{
		wnd->SetResolution(CU::Vec2Ui(640, 360));
		ISTE::BaseEditor::myWantToUpdateSize = true;
	}
	if (ImGui::Selectable("800p 1:1"))
	{
		wnd->SetResolution(CU::Vec2Ui(800, 800));
		ISTE::BaseEditor::myWantToUpdateSize = true;
	}

	ImGui::End();


}
