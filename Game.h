#pragma once

#include <Windows.h>
#include <d3d11.h>
#include <vector>
#include "GameFSM.h"

class Game
{
public:
	static Game &GetInstance();
	~Game() = default;
	void Initialize(HINSTANCE hInstance);
	int Run();
	HWND GetWindow() { return mWindow; }
private:
	Game() = default;
	void InitializeWindow(HINSTANCE hInstance, int windowWidth, int windowHeight);
	void InitializeGame();
	void Loop();
	void ProcessInput();
	void UpdateState();
	void Render();
	HWND mWindow;
};


