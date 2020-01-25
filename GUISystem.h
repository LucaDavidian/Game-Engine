#ifndef GUISYSTEM_H
#define GUISYSTEM_H

#include "data structures/Vector.h"

#ifdef FAST_DELEGATES
	#include "fast delegates/delegate.hpp"
#else
	#include "delegates/delegate.hpp"
#endif  // FAST_DELEGATES

class GUI;

class GUISystem
{
public:
	static GUISystem &GetInstance() { static GUISystem instance; return instance; }

	~GUISystem() { if (mConnection) mConnection->Disconnect(); delete mConnection; }

	void AddGUI(GUI *gui) { mGUIs.InsertLast(gui); }

	void Update();

	Vector<GUI*> *GetGUIs() { return &mGUIs; }

	void OnMouseMove(int x, int y);
	void OnMousePress(int x, int y);
	void OnMouseRelease(int x, int y);
	void OnKeyPress(int key);

	void Connect(ConnectionBase *connection) { mConnection = connection; }
private:
	GUISystem();

	void GetFocused(int x, int y);
	Vector<GUI*> mGUIs;        // GUI are ordered from back to front - last GUI is active (sinks input)
	GUI *mFocused = nullptr;   // needed by GUISystem to select active GUI

	ConnectionBase *mConnection;
};

#endif  // GUISYSTEM_H

