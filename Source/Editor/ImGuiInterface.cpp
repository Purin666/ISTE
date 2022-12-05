#include "ImGuiInterface.h"
#include "ISTE/Context.h"
#include "ISTE/Engine.h"
#include "ISTE/Graphics/DX11.h"


ImGuiInterface::~ImGuiInterface()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
	
	myImGuiIO = nullptr;
}

void ImGuiInterface::Init()
{
	ISTE::Context* engineCtx = ISTE::Context::Get();
	ImGuiLoadFont(myFontAtlas, "segoeui.ttf", 18.0f);
	myFontAtlas.Build();

	ImGui::CreateContext(&myFontAtlas);

	myImGuiIO = &ImGui::GetIO();
	myImGuiIO->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	myImGuiIO->ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; // marcus
	//myImGuiIO->ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	myImGuiIO->IniFilename = "../Assets/EditorSettings/ImGui/ImGui.ini";
	myImGuiIO->LogFilename = nullptr;

	ImGui_ImplWin32_Init(*engineCtx->myEngine->GetWindowsHandle());
	ImGui_ImplDX11_Init(engineCtx->myDX11->GetDevice(), engineCtx->myDX11->GetContext());
	ImGui::StyleColorsLight();
}

void ImGuiInterface::BeginFrame()
{
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame(); 
}

void ImGuiInterface::RenderData(ImDrawData* someData)
{
	ImGui_ImplDX11_RenderDrawData(someData);
}

void ImGuiInterface::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	ImGui::UpdatePlatformWindows(); // marcus
	ImGui::RenderPlatformWindowsDefault(); // marcus
}

ImFont* ImGuiInterface::ImGuiLoadFont(ImFontAtlas& atlas, const char* name, float size, const ImVec2& displayOffset)
{
	char* windir = nullptr;
	if (_dupenv_s(&windir, nullptr, "WINDIR") || windir == nullptr)
		return nullptr;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0104, 0x017C, // Polish characters and more
		0,
	};

	ImFontConfig config;
	config.OversampleH = 4;
	config.OversampleV = 4;
	config.PixelSnapH = false;

	auto path = std::string(windir) + "\\Fonts\\" + name;
	auto font = atlas.AddFontFromFileTTF(path.c_str(), size, &config, ranges);
	if (font)
		font->DisplayOffset = displayOffset;

	free(windir);

	return font;
}
