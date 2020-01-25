#pragma once

#include "GUIComponent.h"
#include "GUIPanel.h"
#include "GUIButton.h"
#include "GUILabel.h"
#include "GUITextField.h"

class ConnectionBase;

class FocusGUIVisitor : public GUIVisitor
{
public:
	FocusGUIVisitor(int x, int y) : x(x), y(y), mFocusedComponent(nullptr) {}
	void Visit(GUIPanel*) override;
	void Visit(GUIButton*) override;
	void Visit(GUILabel*) override {}  // ignore 
	void Visit(GUITextField*) override;
	GUIComponent * GetFocusedComponent() { return mFocusedComponent; }
private:
	int x, y;
	GUIComponent *mFocusedComponent;
};

class GUI
{
public:
	GUI(GUIComponent *root);
	~GUI();

	GUIComponent *GetRoot() { return mRoot; }

	void SetVisible(bool visible) { mRoot->SetVisible(visible); }
	bool IsVisible() const { return mRoot->IsVisible(); }

	bool IsInside(int x, int y) { return mRoot->IsInside(x, y); }
	
	void Destroy() { mIsDeleted = true; }
	bool IsDeleted() { return mIsDeleted; }
	
	void OnMouseMove(int x, int y);
	void OnMousePress(int x, int y);
	void OnMouseRelease(int x, int y);
	void OnKeyPress(int key);

private:
	bool mIsDeleted = false;

	GUIComponent *mRoot;
	GUIComponent *mFocusedComponent = nullptr;
	GUIComponent *mSelectedComponent = nullptr;
};









