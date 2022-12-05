#pragma once
#define WIN32_LEAN_AND_MEAN 
#define NOMINMAX 
#include <windows.h>

#include <ISTE/Math/Vec2.h>

#include <functional> // std::function
using callback_function_wndProc = std::function<LRESULT(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)>;

enum class WindowSetting
{
	Overlapped,
	Borderless,
};


namespace ISTE
{
	struct Context;

	class WindowsWindow
	{
	public:
		WindowsWindow();
		~WindowsWindow();

		const bool Init(const CU::Vec2Ui& aWindowSize);
	
		inline const HWND GetWindowsHandle() const { return myWindowHandle; }

		void SetFullscreen(const bool aFlag);
		void SetResolution(const CU::Vec2Ui& aResolution, const bool aFullScreen = false);
		void ToggleFullscreen();
		void Close();
		
		const bool GetFullscreen() const;
		inline const CU::Vec2Ui GetResolution() const { return myResolution; }
		inline const unsigned int GetWidth() const { return myResolution.x; }
		inline const unsigned int GetHeight() const { return myResolution.y; }

	private:
		static LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

		const CU::Vec2Ui GetDesktopResolution();
		void UpdateWindowSizeChanges();
		void CalculateRatios();

		HWND myWindowHandle;
		callback_function_wndProc myWndProcCallback;
		WNDCLASSEX myWindowClass;

		CU::Vec2Ui myResolution;
		CU::Vec2Ui myResolutionWithBorderDifference;
		float myWindowRatio;
		float myWindowRatioInversed;
		CU::Vec2Ui myWindowRatioVec;
		CU::Vec2Ui myWindowRatioInversedVec;

		Context* myContext;
};
}

