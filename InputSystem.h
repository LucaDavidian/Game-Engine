#ifndef INPUT_SYSTEM_H
#define INPUT_SYSTEM_H

#include <Windows.h>
#include <string>
#include <vector>
#include <map>

#ifdef FAST_DELEGATES
	#include "fast delegates/delegate.hpp"
#else
	#include "delegates/delegate.hpp"
#endif  // FAST_DELEGATES

class InputSystem
{
private:
	MULTICAST_DELEGATE_TWO_PARAM(MouseDelegate, int, int);
	MULTICAST_DELEGATE_ONE_PARAM(KeyboardDelegate, int);
friend LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

public:
	enum class KeyState { JUST_PRESSED, PRESSED, JUST_RELEASED, RELEASED };

	static InputSystem &GetInstance();
	~InputSystem() = default;

	void Initialize(HWND window);
	void PollInput();
	void ProcessInput();
	
	KeyState GetKeyState(unsigned int key) const;
	POINT GetMouseAbsolutePosition() const;
	POINT GetMouseDeltaPosition() const;

#ifdef FAST_DELEGATES
	template <typename T, void (T::*MemFunPtr)(int, int)>
	void RegisterMousePressEvent(T *instance) { mMousePressEvent.Bind<T,MemFunPtr>(*instance); }
	template <typename T, void (T::*MemFunPtr)(int, int)>
	void RegisterMouseReleaseEvent(T *instance) { mMouseReleaseEvent.Bind<T,MemFunPtr>(*instance); }
	template <typename T, void (T::*MemFunPtr)(int, int)>
	void RegisterMouseMoveEvent(T *instance) { mMouseMoveEvent.Bind<T,MemFunPtr>(*instance); }
	template <typename T, void (T:: * MemFunPtr)(int)>
	void RegisterKeyPressEvent(T *instance) { mKeyPressEvent.Bind<T, MemFunPtr>(*instance); }
#else
	template <typename T>
	void RegisterMousePressEvent(T *instance, void (T::*ptr)(int, int)) { mMousePressEvent.Bind(*instance, ptr); }
	template <typename T>
	void RegisterMouseReleaseEvent(T *instance, void (T::*ptr)(int, int)) { mMouseReleaseEvent.Bind(*instance, ptr); }
	template <typename T>
	void RegisterMouseMoveEvent(T *instance, void (T::*ptr)(int, int)) { mMouseMoveEvent.Bind(*instance, ptr); }
	template <typename T>
	void RegisterKeyPressEvent(T *instance, void (T::*ptr)(int)) { mKeyPressEvent.Bind(*instance, ptr); }
	template <typename T>
	void RegisterKeyDownEvent(T *instance, void (T::*ptr)(int)) { mKeyDownEvent.Bind(*instance, ptr); }
#endif

private:
	InputSystem() = default;
	
	void GetKeyboardState();
	static const int NUM_KEYS = 256;
	unsigned int mCurrentKeys[NUM_KEYS];
	unsigned int mPreviousKeys[NUM_KEYS];

	KeyboardDelegate mKeyPressEvent;
	KeyboardDelegate mKeyDownEvent;
	
	void GetMouseState();
	POINT mMousePreviousPosition;
	POINT mMouseCurrentPosition;
	HWND mWindow;

	MouseDelegate mMouseMoveEvent;
	MouseDelegate mMousePressEvent;
	MouseDelegate mMouseReleaseEvent;

	LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
};

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif  // INPUT_SYSTEM_H

