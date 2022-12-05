#include "InputHandler.h"

#pragma region InputHandler

#include "ISTE/CU/MemTrack.hpp"

bool CU::InputHandler::UpdateEvents(const HWND& aHWnd, const UINT& aMessage, const WPARAM& aWParam, const LPARAM& /*aLParam*/)
{
	//Lock cursor to the center of the window
	if (myCursorLock)
	{
		RECT WindowRect;
		GetWindowRect(aHWnd, &WindowRect);
		const int centerX = (WindowRect.right + WindowRect.left) / 2;
		const int centerY = (WindowRect.top + WindowRect.bottom) / 2;
		SetCursorPos(centerX, centerY);
	}
	switch (aMessage)
	{
		//Down
	case WM_KEYDOWN:
		myCurrentKeyStates[aWParam] = true;
		break;
	case WM_LBUTTONDOWN:
		myCurrentKeyStates[MK_LBUTTON] = true;
		break;
	case WM_MBUTTONDOWN:
		myCurrentKeyStates[255] = true;
		break;
	case WM_RBUTTONDOWN:
		myCurrentKeyStates[MK_RBUTTON] = true;
		break;
	case WM_MOUSEWHEEL:
		myScrollCount = GET_WHEEL_DELTA_WPARAM(aWParam);
		break;
		//Up
	case WM_KEYUP:
		myCurrentKeyStates[aWParam] = false;
		break;
	case WM_LBUTTONUP:
		myCurrentKeyStates[MK_LBUTTON] = false;
		break;
	case WM_MBUTTONUP:
		myCurrentKeyStates[255] = false;
		break;
	case WM_RBUTTONUP:;
		myCurrentKeyStates[MK_RBUTTON] = false;
		break;
		//Mouse move
	case WM_MOUSEMOVE:
		myLastMousePos = myCurrentMousePos;
		GetCursorPos(&myCurrentMousePos);
		break;
	default:
		return false; //Not a valid message found
	}
	return true;
}

CU::InputHandler::~InputHandler()
{
}


int CU::InputHandler::GetScrollCount() const
{
	return myScrollCount;
}

bool CU::InputHandler::IsKeyDown(const unsigned char& aKeyCode) const
{
	//The key IS pressed now, but it was NOT pressed the previous time
	return myCurrentKeyStates[aKeyCode] && !myPreviousKeyStates[aKeyCode];
}

bool CU::InputHandler::IsKeyUp(const unsigned char& aKeyCode) const
{
	//The key is NOT pressed now, but WAS pressed the previous time
	return !myCurrentKeyStates[aKeyCode] && myPreviousKeyStates[aKeyCode];
}

bool CU::InputHandler::IsKeyHeldDown(const unsigned char& aKeyCode) const
{
	return myCurrentKeyStates[aKeyCode];
}

bool CU::InputHandler::IsAnyKeyDown() const
{
	return myCurrentKeyStates.any();
}

void CU::InputHandler::SetCursorVisibility(const bool& aIsVisible)
{
	if (aIsVisible == myCursorVisible)
	{
		return;
	}

	ShowCursor(aIsVisible);
	myCursorVisible = aIsVisible;
}

void CU::InputHandler::SetCursorLock(const bool& aIsLocked)
{
	myCursorLock = aIsLocked;
}

void CU::InputHandler::SetMousePos(int aPosX, int aPosY)
{
	SetCursorPos(aPosX, aPosY);
}

POINT CU::InputHandler::GetMouseVelocity() const
{
	POINT velocityPoint;
	velocityPoint.x = myCurrentMousePos.x - myLastMousePos.x;
	velocityPoint.y = myCurrentMousePos.y - myLastMousePos.y;
	return velocityPoint;
}

void CU::InputHandler::Refresh()
{
	myPreviousKeyStates = myCurrentKeyStates;
	myLastMousePos = myCurrentMousePos;
	myScrollCount = 0;
}

void CU::InputHandler::SetCursorCurrentPos(POINT aPos)
{
	myCurrentMousePos = aPos;
	myLastMousePos = aPos;
}

// pos 0 = x, pos 1 = y
POINT CU::InputHandler::GetMouseRel(unsigned int* aWindowSize, unsigned int* aRenderSize)
{
	POINT myMousePos;

	RECT rect;
	POINT p;

	p.x = 0;
	p.y = 0;

	if (GetWindowRect(GetActiveWindow(), &rect))
	{
		GetCursorPos(&myMousePos);

		p.x = myMousePos.x - rect.left - 7;
		p.y = myMousePos.y - rect.top - 32; //-32 cause top bar

		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top - 32;


		float tRatio = (float)aWindowSize[0] / (float)aWindowSize[1];
		float rRatio = (float)width / (float)height;

		int offsetY = 0;
		int offsetX = 0;

		if (rRatio < tRatio)
		{
			offsetY = (height - aRenderSize[1]) / 2;
		}
		else if (rRatio > tRatio)
		{

			offsetX = (width - (int)aRenderSize[0]) / 2;

		}

		p.x -= (LONG)offsetX;
		p.y -= (LONG)offsetY;
	}

	return p;
}

#pragma endregion