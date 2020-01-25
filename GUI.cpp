#include "GUI.h"
#include "GUISystem.h"

void FocusGUIVisitor::Visit(GUIPanel *component)
{
	if (component->IsInside(x, y))
		mFocusedComponent = component;
}

void FocusGUIVisitor::Visit(GUIButton *component)
{
	if (component->IsInside(x, y))
		mFocusedComponent = component;
}

void FocusGUIVisitor::Visit(GUITextField *component)
{
	if (component->IsInside(x, y))
		mFocusedComponent = component;
}

GUI::GUI(GUIComponent *root) : mRoot(root) 
{
	// register with GUI system
	GUISystem::GetInstance().AddGUI(this);
}

GUI::~GUI() 
{ 
	delete mRoot; 
}

void GUI::OnMouseMove(int x, int y)
{
	static int previousX = x;
	static int previousY = y;

	FocusGUIVisitor v(x, y);
	mRoot->AcceptPreorder(v);
	GUIComponent *focusedComponent = v.GetFocusedComponent();

	if (focusedComponent != mFocusedComponent)  // focus has changed
	{
		if (mSelectedComponent)
		{
			if (focusedComponent == mSelectedComponent)
				mSelectedComponent->OnMouseEnter();

			if (mFocusedComponent == mSelectedComponent)
				mSelectedComponent->OnMouseLeave();

			mFocusedComponent = focusedComponent;
		}
		else  // if no component is selected
		{
			if (mFocusedComponent)
				mFocusedComponent->OnMouseLeave();

			mFocusedComponent = focusedComponent;

			if (mFocusedComponent)
				mFocusedComponent->OnMouseEnter();
		}
	}

	if (mSelectedComponent)
		mSelectedComponent->OnMouseMove(x - previousX, y - previousY);

	previousX = x;
	previousY = y;
}

void GUI::OnMousePress(int x, int y)
{
	if (mFocusedComponent)
	{
		if (mSelectedComponent && mFocusedComponent != mSelectedComponent)
			mSelectedComponent->OnMousePress();

		mSelectedComponent = mFocusedComponent;
		mSelectedComponent->OnMousePress();
	}
}

void GUI::OnMouseRelease(int x, int y)
{
	if (mSelectedComponent)
	{
		mSelectedComponent->OnMouseRelease();

		if (!mSelectedComponent->IsSelected())
		{
			mSelectedComponent = nullptr;

			if (mFocusedComponent)
				mFocusedComponent->OnMouseEnter();
		}
	}	
}

void GUI::OnKeyPress(int key)
{
	if (mSelectedComponent)
		mSelectedComponent->OnKeyPress(key);
}

