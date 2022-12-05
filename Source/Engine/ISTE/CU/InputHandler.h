#pragma once

#include <bitset>
#include <Windows.h>

namespace CU {

	class InputHandler
	{
	private:
		std::bitset<256> myCurrentKeyStates;
		std::bitset<256> myPreviousKeyStates;

		POINT myCurrentMousePos = {};
		POINT myLastMousePos = {};

		bool myCursorLock = false;
		bool myCursorVisible = true;
		int myScrollCount = 0;
	public:
		bool UpdateEvents(const HWND& aHWnd, const UINT& aMessage, const WPARAM& aWParam, const LPARAM& aLParam);
		~InputHandler();
		//Keyboard and Mouse buttons. Use MK for mouse buttons e.g. MK_LBUTTON
		bool IsKeyDown(const unsigned char& aKeyCode) const;
		bool IsKeyUp(const unsigned char& aKeyCode) const;
		bool IsKeyHeldDown(const unsigned char& aKeyCode) const;
		bool IsAnyKeyDown() const;
		int GetScrollCount() const;
		//Mouse
		void SetCursorVisibility(const bool& aIsVisible);
		void SetCursorLock(const bool& aIsLocked);
		void SetMousePos(int aPosX, int aPosY);
		POINT GetMouseVelocity() const;
		POINT GetMouseRel(unsigned int* aWindowSize, unsigned int* aRenderSize);
		void SetCursorCurrentPos(POINT aPos);

		void Refresh();
	};
}