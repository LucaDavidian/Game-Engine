#include "InputSystem.h"

InputSystem &InputSystem::GetInstance()
{
	static InputSystem instance;

	return instance;
}

void InputSystem::Initialize(HWND window)
{
	mWindow = window;
}

//  save the previous state of the keyboard and get the current state 
void InputSystem::GetKeyboardState()
{
	// update current keyboard state
	for (unsigned int key = 0; key < NUM_KEYS; key++)
	{
		// save previous key state
		mPreviousKeys[key] = mCurrentKeys[key];

		// update current key state
		unsigned int isPressed = GetAsyncKeyState(key) & 0x8000 ? 1 : 0;  // GetAsyncKeyState polls the current state of the keyboard
		mCurrentKeys[key] = isPressed;
	}
}

// get the state of a key
InputSystem::KeyState InputSystem::GetKeyState(unsigned int key) const
{
	if (mPreviousKeys[key] == 1)   // key was pressed during previous frame
	{
		if (mCurrentKeys[key] == 1)
			return KeyState::PRESSED;
		else
			return KeyState::JUST_RELEASED;
	}
	else   // key wasn't pressed during the previous frame
	{
		if (mCurrentKeys[key] == 1)
			return KeyState::JUST_PRESSED;
		else
			return KeyState::RELEASED;
	}
}

// update mouse position
void InputSystem::GetMouseState()
{
	mMousePreviousPosition = mMouseCurrentPosition;           // save previous mouse position relative to client area
	GetCursorPos(&mMouseCurrentPosition);                     // get mouse position in absolute (screen) coordinates
	ScreenToClient(mWindow, &mMouseCurrentPosition);          // get coordinates relative to window client area
}


POINT InputSystem::GetMouseDeltaPosition() const
{
	POINT deltaPos;
	deltaPos.x = mMouseCurrentPosition.x - mMousePreviousPosition.x;
	deltaPos.y = mMouseCurrentPosition.y - mMousePreviousPosition.y;

	return deltaPos;
}

POINT InputSystem::GetMouseAbsolutePosition() const
{
	return mMouseCurrentPosition;
}

void InputSystem::PollInput()
{
	// poll keyboard state
	GetKeyboardState();

	// poll mouse state
	GetMouseState();
}

#include <ctype.h>

void InputSystem::ProcessInput()
{
	if (GetMouseDeltaPosition().x != 0 || GetMouseDeltaPosition().y != 0)
		mMouseMoveEvent.Invoke(mMouseCurrentPosition.x, mMouseCurrentPosition.y);

	if (GetKeyState(VK_LBUTTON) == KeyState::JUST_PRESSED)
		mMousePressEvent.Invoke(mMouseCurrentPosition.x, mMouseCurrentPosition.y);

	if (GetKeyState(VK_LBUTTON) == KeyState::JUST_RELEASED)
		mMouseReleaseEvent.Invoke(mMouseCurrentPosition.x, mMouseCurrentPosition.y);

	for (int vk = 0; vk < NUM_KEYS; vk++)
		if (GetKeyState(vk) == KeyState::JUST_PRESSED)  // vk is the virtual key-code
		{
			char key;

			if (vk == VK_LBUTTON || vk == VK_SHIFT || vk == VK_LSHIFT || vk == VK_RSHIFT)
				continue;
			else if (vk == VK_RETURN)
				key = '\n';
			else
				key = MapVirtualKey(vk, MAPVK_VK_TO_CHAR);

			if (GetKeyState(VK_SHIFT) != KeyState::PRESSED)
				key = tolower(key);

			mKeyPressEvent.Invoke(key);
		}
		else if (GetKeyState(vk) == KeyState::PRESSED)  // vk is the virtual key-code
		{
			int key = MapVirtualKey(vk, MAPVK_VK_TO_CHAR);

			if (vk == VK_LEFT || vk == VK_RIGHT || vk == VK_UP || vk == VK_DOWN || vk == VK_PRIOR || vk == VK_NEXT)
				mKeyDownEvent.Invoke(vk);
			else
				mKeyDownEvent.Invoke(vk);
		}
}

// input main callback function
LRESULT InputSystem::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_SYSKEYDOWN:
		case WM_SYSKEYUP:
		case WM_KEYDOWN:
		case WM_KEYUP:
		{
			bool isDown = !(lParam & 1 << 31);
			bool wasDown = lParam & 1 << 30;
			if (wParam == 'O' && isDown && wasDown)
			{
		
			}
		}
			return 0;
		case WM_CHAR:
			// switch over keyboard characters
			return 0;
		case WM_LBUTTONDOWN:
			//mMousePressedEvent.Invoke(mMouseCurrentPosition.x, mMouseCurrentPosition.y);
			return 0;
		case WM_RBUTTONDOWN:
			return 0;
		case WM_MBUTTONDOWN:
			return 0;
		case WM_MOUSEWHEEL:
			return 0;
		case WM_MOUSEMOVE:
			//mMouseMovedEvent.Invoke(mMouseCurrentPosition.x, mMouseCurrentPosition.y);
			return 0;
		default:
			return DefWindowProc(hWnd, msg, wParam, lParam);
	}
}

// Windows event handler
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CLOSE:
		if (MessageBox(hWnd, L"are you sure you want to quit?", L"Quit", MB_YESNO) == IDYES)
			PostQuitMessage(0);
		return 0;
	default:
		return InputSystem::GetInstance().WndProc(hWnd, msg, wParam, lParam);    // delegate to input system class 
	}
}


