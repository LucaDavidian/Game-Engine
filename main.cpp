#include <Windows.h>
#include "Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR commandLine, int show)
{
	Game::GetInstance().Initialize(hInstance);
	return Game::GetInstance().Run();
}