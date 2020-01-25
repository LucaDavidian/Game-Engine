#include "Game.h"
#include "Timer.h"
#include "Error.h"

#include "GraphicsSystem.h"
#include "InputSystem.h"
#include "RenderingSystem.h"
#include "PhysicsSystem.h"
#include "CollisionSystem.h"
#include "EntitySystem.h"
#include "GUISystem.h"

Game &Game::GetInstance()
{
	static Game instance;
	return instance;
}

void Game::Initialize(HINSTANCE hInstance)
{
	int windowWidth = 1024;
	int windowHeight = 768;

	InitializeWindow(hInstance, windowWidth, windowHeight);   // create window
	GraphicsSystem::GetInstance().Initialize(mWindow);        // initialize graphics system
	InputSystem::GetInstance().Initialize(mWindow);           // initialize input system
	InitializeGame();                                         // initialize game
}

void Game::InitializeWindow(HINSTANCE hInstance, int windowWidth, int windowHeight)
{
	// create window class
	WNDCLASS windowClass = {};
	windowClass.hInstance = hInstance;
	windowClass.lpfnWndProc = &WndProc;    // declared in InputSystem.h
	windowClass.lpszClassName = L"GameEngineWindowClass";
	windowClass.lpszMenuName = NULL;
	windowClass.style = CS_HREDRAW | CS_VREDRAW;
	windowClass.cbClsExtra = 0;
	windowClass.cbWndExtra = 0;
	windowClass.hCursor = LoadCursor(NULL, IDC_ARROW);
	windowClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	windowClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);

	// register window class
	RegisterClass(&windowClass);

	RECT rect = { 0, 0, (LONG)windowWidth, (LONG)windowHeight };

	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, false);

	mWindow = CreateWindow(L"GameEngineWindowClass", 0, 0, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, 0, 0, hInstance, 0);
	if (!mWindow)
		ErrorBox("window creation failed");

	ShowWindow(mWindow, SW_SHOW);
	UpdateWindow(mWindow);
}

#include "GameFSM.h"

void Game::InitializeGame()
{
	Timer::GetInstance().Reset();
	GameFSM::GetInstance().Init();
}

int Game::Run()
{
	MSG msg;

	do
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE) != 0)  // Windows message pump
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		Loop();  

	} while (msg.message != WM_QUIT);

	return msg.wParam;
}

void Game::Loop()
{
	ProcessInput();
	UpdateState();
	Render();
}

void Game::ProcessInput()
{
	InputSystem::GetInstance().PollInput();  // poll state of input devices 
}

void Game::UpdateState()
{
	// update game time
	Timer::GetInstance().Tick();
	float dt = Timer::GetInstance().GetDeltaTime();

	//GameFSM::GetInstance().OnEvent(EventSystem::GetInstance().GetEvent());

	PhysicsSystem::GetInstance().Update(dt);         // update physics
	CollisionSystem::GetInstance().DoCollisions();   // perform collision  detection and resolution
	InputSystem::GetInstance().ProcessInput();       // invoke delegates
	EntitySystem::GetInstance().Update();            // remove dead entities
	GUISystem::GetInstance().Update();               // remove destroyed GUI
}

void Game::Render()
{
	// clear back buffer and depth/stencil buffer 
	const float color[] = { 0.1f, 0.2f, 0.1f, 1.0f };
	GraphicsSystem::GetInstance().ClearScreen(color);

	// render scene
	RenderingSystem::GetInstance().Render();

	// present back buffer 
	GraphicsSystem::GetInstance().Present();
}





