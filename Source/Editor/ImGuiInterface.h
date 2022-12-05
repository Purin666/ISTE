#pragma once
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"

class ImGuiInterface
{
public:
	ImGuiInterface() = default;
	~ImGuiInterface();

	void Init();
	void BeginFrame();
	void RenderData(ImDrawData* someData);
	void Render();

	ImFont* ImGuiLoadFont(ImFontAtlas& atlas, const char* name, float size, const ImVec2& displayOffset = ImVec2(0, 0)); 
private:
	ImGuiIO* myImGuiIO;
	ImFontAtlas myFontAtlas;

};

