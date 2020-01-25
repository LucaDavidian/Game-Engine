#include "GUISystem.h"
#include "GUI.h"
#include "InputSystem.h"

GUISystem::GUISystem()
{
	// register with input system
#ifdef FAST_DELEGATES
	InputSystem::GetInstance().RegisterMouseMoveEvent<GUI, &GUI::OnMouseMove>(this);
	InputSystem::GetInstance().RegisterMousePressEvent<GUI, &GUI::OnMousePress>(this);
	InputSystem::GetInstance().RegisterMouseReleaseEvent<GUI, &GUI::OnMouseRelease>(this);
	InputSystem::GetInstance().RegisterKeyPressEvent<GUI, &GUI::OnKeyPress>(this);
#else
	InputSystem::GetInstance().RegisterMouseMoveEvent(this, &GUISystem::OnMouseMove);
	InputSystem::GetInstance().RegisterMousePressEvent(this, &GUISystem::OnMousePress);
	InputSystem::GetInstance().RegisterMouseReleaseEvent(this, &GUISystem::OnMouseRelease);
	InputSystem::GetInstance().RegisterKeyPressEvent(this, &GUISystem::OnKeyPress);
#endif
}

void GUISystem::Update()
{
	// delete marked GUIs
	auto it = mGUIs.begin();

	while (it != mGUIs.end())
		if ((*it)->IsDeleted())
		{
			delete *it;
			it = mGUIs.Remove(it);
		}
		else
			++it;
}

void GUISystem::GetFocused(int x, int y)
{
	mFocused = nullptr;

	auto it = mGUIs.begin();

	while (it != mGUIs.end())
	{
		if ((*it)->IsVisible() && (*it)->IsInside(x, y))
			mFocused = *it;  // the last GUI has focus (mGUIs.back() is always the foremost GUI)

		++it;
	}

	//for (GUI *gui : mGUIs)
	//	if (gui->IsVisible() && gui->IsInside(x, y))
	//		mFocused = gui;
}

void GUISystem::OnMouseMove(int x, int y)  // which VISIBLE GUI has focus?
{
	if (mGUIs.Empty())
		return;

	GetFocused(x, y);

	mGUIs.Last()->OnMouseMove(x, y);
}

void GUISystem::OnMousePress(int x, int y)
{
	if (mGUIs.Empty())
		return;

	GetFocused(x, y);

	if (mFocused)  // if mouse is over GUI
	{		
		if (mFocused != mGUIs.Last())  // change active GUI - move selected GUI to back of array
		{
			auto it = mGUIs.begin();

			while (it != mGUIs.end())
			{
				if (*it == mFocused)
				{
					mGUIs.Remove(it);
					break;
				}

				++it;
			}

			mGUIs.InsertLast(mFocused);
			mGUIs.Last()->OnMouseMove(x, y);
		}

		mGUIs.Last()->OnMousePress(x, y);  // dispatch event		
	}
}

void GUISystem::OnMouseRelease(int x, int y)
{
	if (mGUIs.Empty())
		return;

	GUI *temp = mGUIs.Last();

	mGUIs.Last()->OnMouseRelease(x, y);  // A new GUI might be created or an old GUI might be hidden

	if (!mGUIs.Last()->IsVisible())  // active GUI is hidden
	{
		GUI *temp = mGUIs.Last();
		mGUIs.RemoveLast();
		mGUIs.InsertFirst(temp);
	}

	if (temp != mGUIs.Last())     // if active GUI has changed (new GUI created or active GUI hidden) exit from previous active GUI
		temp->OnMouseMove(0, 0);  

	mGUIs.Last()->OnMouseMove(x, y);  // focus GUI
}

void GUISystem::OnKeyPress(int key)
{
	if (mGUIs.Empty())
		return;

	mGUIs.Last()->OnKeyPress(key);
}