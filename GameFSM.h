#pragma once

#include "data structures/Stack.h"
#include "EventSystem.h"

#ifdef FAST_DELEGATES
	#include "fast delegates/delegate.hpp"
#else
	#include "delegates/delegate.hpp"
#endif  // FAST_DELEGATES

#include "data structures/Vector.h"

class GameFSM;

class GameState
{
friend class GameFSM;
public:
	virtual ~GameState() = default;

	virtual void OnEntry() = 0;
	virtual void OnExit() = 0;
	virtual void OnButtonPressed(GameFSM &fsm, const std::string &buttonName) = 0;
	virtual void OnKeyPress(GameFSM &fsm, int key) {}
protected:
	GameState() = default;

	static class MenuGameState mMenuGameState;
	static class PlayGameState mPlayGameState;
	static class PauseGameState mPauseGameState;
	static class ExitGameState mExitGameState;
};

class MenuGameState : public GameState
{
public:
	void OnEntry() override;
	void OnExit() override;
	void OnButtonPressed(GameFSM &fsm, const std::string &buttonName) override;
private:
	class GUI *mMenu;
};

class PlayGameState : public GameState
{
public:
	void OnEntry() override;
	void OnExit() override;
	void OnButtonPressed(GameFSM &fsm, const std::string &buttonName) override;
	void OnKeyPress(GameFSM &fsm, int key) override;
private:
	class GUI *mGameGUI;
	Vector<class Entity*> mEntities;
	class Picker *mPicker;
};

class PauseGameState : public GameState
{
public:
	void OnEntry() override;
	void OnExit() override;
	void OnButtonPressed(GameFSM &fsm, const std::string &buttonName) override;
private:
	class GUI *mPauseMenu;
};

class ExitGameState : public GameState
{
public:
	void OnEntry() override;
	void OnExit() override;
	void OnButtonPressed(GameFSM &fsm, const std::string &buttonName) override;
private:
	class GUI *mExitMenu;
};

#include "InputSystem.h"

class GameFSM
{
public:
	~GameFSM() { if (mConnection) mConnection->Disconnect(); delete mConnection; }

	static GameFSM &GetInstance() { static GameFSM instance; return instance; }

	void Init() { PushState(&GameState::mMenuGameState); }

	void ChangeState(GameState *newState) { mCurrentState.Top()->OnExit(); mCurrentState.Pop(); mCurrentState.Push(newState); mCurrentState.Top()->OnEntry(); }
	void PushState(GameState *newState) { mCurrentState.Push(newState); mCurrentState.Top()->OnEntry(); }
	void PopState() { mCurrentState.Top()->OnExit(); mCurrentState.Pop(); }

	void OnButtonPressed(const std::string &buttonName) { mCurrentState.Top()->OnButtonPressed(*this, buttonName); }
	void OnKeyPress(int key) { mCurrentState.Top()->OnKeyPress(*this, key); }

	void Connect(ConnectionBase *connection) { mConnection = connection; }
private:
	GameFSM() { InputSystem::GetInstance().RegisterKeyPressEvent(this, &GameFSM::OnKeyPress); }

	Stack<GameState*> mCurrentState;

	ConnectionBase *mConnection;
};

