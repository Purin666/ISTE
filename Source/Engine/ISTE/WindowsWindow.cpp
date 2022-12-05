#include "WindowsWindow.h"
#include <WinUser.h>
#include "Context.h"
#include "imgui/imgui_impl_win32.h"
#include "CU/InputHandler.h"
#include "ISTE/Graphics/DX11.h"
#include "ISTE/Graphics/GraphicsEngine.h"
#include "ISTE/ECSB/SystemManager.h"
#include "ISTE/Graphics/ComponentAndSystem/CameraSystem.h"
#include <dxgi.h>  

#include "ISTE/CU/MemTrack.hpp"

ISTE::WindowsWindow::WindowsWindow()
	: myContext(Context::Get())
{
	myWindowHandle = 0;
	myResolution = { 0,0 };
	myResolutionWithBorderDifference = { 0,0 };
	myWindowRatio = 0.f;
	myWindowRatioInversed = 0.f;
	myWindowRatioVec = { 0,0 };
	myWindowRatioInversedVec = { 0,0 };
}

ISTE::WindowsWindow::~WindowsWindow()
{
	if (myWindowHandle) Close();
}

const bool ISTE::WindowsWindow::Init(const CU::Vec2Ui& aWindowSize/*, callback_function_wndProc aWndPrcCallback*/)
{
	//myWndProcCallback = aWndPrcCallback;

	HINSTANCE instance = GetModuleHandle(NULL);
	ZeroMemory(&myWindowClass, sizeof(WNDCLASSEX));
	myWindowClass.cbSize = sizeof(WNDCLASSEX);
	myWindowClass.style = CS_HREDRAW | CS_VREDRAW;
	myWindowClass.lpfnWndProc = WindowProc;
	myWindowClass.hInstance = instance;
	myWindowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	myWindowClass.hbrBackground = (HBRUSH)COLOR_WINDOW;
	myWindowClass.lpszClassName = L"MAIN_WINDOW";
	//myWindowClass.hIcon = ::LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON1));
	//myWindowClass.hIconSm = LoadIcon(instance, MAKEINTRESOURCE(IDI_ICON1));
	RegisterClassEx(&myWindowClass);

	WindowSetting setting = WindowSetting::Overlapped;
	DWORD windowStyle = 0;
	switch (setting)
	{
	case WindowSetting::Overlapped:
		windowStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;
		break;
	case WindowSetting::Borderless:
		windowStyle = WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		break;
	default:
		break;
	}


	myWindowHandle = CreateWindow(L"MAIN_WINDOW", L"Im So Tired Engine",
		windowStyle, CW_USEDEFAULT, CW_USEDEFAULT, aWindowSize.x, aWindowSize.y ,
		nullptr, nullptr, instance, nullptr);
	if (!myWindowHandle) return 0;

	ShowWindow(myWindowHandle, true);
	UpdateWindow(myWindowHandle);

	myResolution = aWindowSize;

	return true;
}

void ISTE::WindowsWindow::SetFullscreen(const bool aFlag)
{
	Context::Get()->myDX11->mySwapChain->SetFullscreenState(aFlag, nullptr);
	SetResolution(GetDesktopResolution(), true);
}

void ISTE::WindowsWindow::SetResolution(const CU::Vec2Ui& aResolution, const bool aFullScreen)
{
	myResolution = aResolution;
	UINT flags = SWP_NOMOVE | SWP_NOOWNERZORDER | SWP_NOZORDER;
	const unsigned int xOff = 16;
	const unsigned int yOff = 40;
	if (aFullScreen)
		::SetWindowPos(myWindowHandle, 0, 0, 0, aResolution.x, aResolution.y, flags);
	else 
		::SetWindowPos(myWindowHandle, 0, 0, 0, aResolution.x + xOff, aResolution.y + yOff, flags);

	Context::Get()->myDX11->SetResolution(aResolution);

	auto camSystem = Context::Get()->mySystemManager->GetSystem<CameraSystem>();
	camSystem->UpdateCameraResolution(aResolution);

	CalculateRatios();
}

void ISTE::WindowsWindow::ToggleFullscreen()
{
	BOOL flag;
	Context::Get()->myDX11->mySwapChain->GetFullscreenState(&flag, nullptr);
	SetFullscreen(!flag);
}

void ISTE::WindowsWindow::Close()
{
	DestroyWindow(myWindowHandle);
}

const bool ISTE::WindowsWindow::GetFullscreen() const
{
	BOOL flag;
	Context::Get()->myDX11->mySwapChain->GetFullscreenState(&flag, nullptr);
	return flag;
}


IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK ISTE::WindowsWindow::WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	MSG msg{
		hWnd,
		message,
		wParam,
		lParam
	};

	//myContext->myInputHandler->UpdateEvents(hWnd, message, wParam, lParam);
	Context* ctx = Context::Get();
	if (ctx != nullptr)
		ctx->myInputHandler->UpdateEvents(hWnd, message, wParam, lParam);

	ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam);

	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}


const CU::Vec2Ui ISTE::WindowsWindow::GetDesktopResolution()
{
	RECT r;
	GetClientRect(myWindowHandle, &r);
	return { static_cast<unsigned int>(r.right - r.left), static_cast<unsigned int>(r.bottom - r.top) };
}

void ISTE::WindowsWindow::UpdateWindowSizeChanges()
{
	//SetResolution(Vector2ui(myCreateParameters.myTargetWidth, myCreateParameters.myTargetHeight), false);
	//SetViewPort(0, 0, myCreateParameters.myTargetWidth, myCreateParameters.myTargetHeight);
}

void ISTE::WindowsWindow::CalculateRatios()
{
	float sizeX = static_cast<float>(myResolution.x);
	float sizeY = static_cast<float>(myResolution.y);
	if (sizeY > sizeX)
	{
		float temp = sizeX;
		sizeX = sizeY;
		sizeY = temp;
	}

	myWindowRatio = static_cast<float>(sizeX) / static_cast<float>(sizeY);
	myWindowRatioInversed = static_cast<float>(sizeY) / static_cast<float>(sizeX);

	myWindowRatioVec.x = 1;
	myWindowRatioVec.y = 1;
	myWindowRatioInversedVec.x = 1;
	myWindowRatioInversedVec.y = 1;
	if (sizeX >= sizeY)
	{
		myWindowRatioVec.y = myWindowRatio;
		myWindowRatioInversedVec.y = myWindowRatioInversed;
	}
	else
	{
		myWindowRatioVec.x = myWindowRatio;
		myWindowRatioInversedVec.x = myWindowRatioInversed;
	}
}
