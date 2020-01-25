#ifndef GUICOMPONENT_H
#define GUICOMPONENT_H

#include <DirectXMath.h>
#include <string>

using namespace DirectX;

class GUIPanel;
class GUIButton;
class GUILabel;
class GUITextField;
class GUI;

class GUIVisitor
{
public:
	~GUIVisitor() = default;
	virtual void Visit(GUIPanel*) = 0;
	virtual void Visit(GUIButton*) = 0;
	virtual void Visit(GUILabel*) = 0;
	virtual void Visit(GUITextField*) = 0;
protected:
	GUIVisitor() = default;
};

class GUIComponent
{
public:
	GUIComponent() : mParent(nullptr), mRelativePositionTL(XMFLOAT2()), mRelativeDimensions(XMFLOAT2(1.0f, 1.0f)) { Update_(); }
	virtual ~GUIComponent() = default;

	virtual void AcceptPreorder(GUIVisitor&) = 0;
	virtual void AcceptPostorder(GUIVisitor&) = 0;

	virtual void Update() { Update_(); }

	void SetRelativePositionTL(const XMFLOAT2 &relativePosition) { mRelativePositionTL = relativePosition; Update(); }
	void SetRelativePositionTR(const XMFLOAT2 &relativePosition) { mRelativePositionTL = XMFLOAT2(relativePosition.x - mRelativeDimensions.x, relativePosition.y); Update(); }
	void SetRelativePositionBL(const XMFLOAT2 &relativePosition) { mRelativePositionTL = XMFLOAT2(relativePosition.x, relativePosition.y - mRelativeDimensions.y); Update(); }
	void SetRelativePositionBR(const XMFLOAT2 &relativePosition) { mRelativePositionTL = XMFLOAT2(relativePosition.x - mRelativeDimensions.x, relativePosition.y - mRelativeDimensions.y); Update(); }
	void SetRelativePositionCenter(const XMFLOAT2 &relativePositionCenter) { mRelativePositionTL.x = relativePositionCenter.x - mRelativeDimensions.x / 2.0f; mRelativePositionTL.y = relativePositionCenter.y - mRelativeDimensions.y / 2.0f; Update(); }
	
	void SetRelativeDimensions(const XMFLOAT2 &relativeDimensions) { mRelativePositionTL = XMFLOAT2(mRelativePositionTL.x + (mRelativeDimensions.x - relativeDimensions.x) / 2.0f, mRelativePositionTL.y + (mRelativeDimensions.y - relativeDimensions.y) / 2.0f); mRelativeDimensions = relativeDimensions; Update(); }

	void SetScreenPositionTL(const XMFLOAT2 &screenPosition);
	void SetScreenPositionTR(const XMFLOAT2 &screenPosition) { SetScreenPositionTL(XMFLOAT2(screenPosition.x - mScreenDimensions.x, screenPosition.y)); }
	void SetScreenPositionBL(const XMFLOAT2 &screenPosition) { SetScreenPositionTL(XMFLOAT2(screenPosition.x, screenPosition.y - mScreenDimensions.y)); }
	void SetScreenPositionBR(const XMFLOAT2 &screenPosition) { SetScreenPositionTL(XMFLOAT2(screenPosition.x - mScreenDimensions.x, screenPosition.y - mScreenDimensions.y)); }
	void SetScreenPositionCenter(const XMFLOAT2 &screenPosition) { SetScreenPositionTL(XMFLOAT2(screenPosition.x - mScreenDimensions.x / 2.0f, screenPosition.y - mScreenDimensions.y / 2.0f)); }

	void SetScreenDimensions(const XMFLOAT2 &screenDimensions);

	XMFLOAT2 GetRelativeDimensions() const { return mRelativeDimensions; }

	XMFLOAT2 GetNormalizedPositionTL() const { return mNormalizedPositionTL; }
	XMFLOAT2 GetNormalizedPositionTR() const { return XMFLOAT2(mNormalizedPositionTL.x + mNormalizedDimensions.x, mNormalizedPositionTL.y); }
	XMFLOAT2 GetNormalizedPositionBL() const { return XMFLOAT2(mNormalizedPositionTL.x, mNormalizedPositionTL.y - mNormalizedDimensions.y); }
	XMFLOAT2 GetNormalizedPositionBR() const { return XMFLOAT2(mNormalizedPositionTL.x + mNormalizedDimensions.x, mNormalizedPositionTL.y - mNormalizedDimensions.y); }
	XMFLOAT2 GetNormalizedPositionCenter() const { return XMFLOAT2(mNormalizedPositionTL.x + mNormalizedDimensions.x / 2.0f, mNormalizedPositionTL.y - mNormalizedDimensions.y / 2.0f); }

	XMFLOAT2 GetNormalizedDimensions() const { return mNormalizedDimensions; }

	XMFLOAT2 GetScreenPositionTL() const { return mScreenPositionTL; }
	XMFLOAT2 GetScreenPositionTR() const { return XMFLOAT2(mScreenPositionTL.x + mScreenDimensions.x, mScreenPositionTL.y); }	
	XMFLOAT2 GetScreenPositionBL() const { return XMFLOAT2(mScreenPositionTL.x, mScreenPositionTL.y + mScreenDimensions.y); }
	XMFLOAT2 GetScreenPositionBR() const { return XMFLOAT2(mScreenPositionTL.x + mScreenDimensions.x, mScreenPositionTL.y + mScreenDimensions.y); }
	XMFLOAT2 GetScreenPositionCenter() const { return XMFLOAT2(mScreenPositionTL.x + mScreenDimensions.x / 2.0f, mScreenPositionTL.y + mScreenDimensions.y / 2.0f); }

	XMFLOAT2 GetScreenDimensions() const { return mScreenDimensions; }

	void Scale(float scaleFactor) { SetRelativeDimensions(XMFLOAT2(GetRelativeDimensions().x * scaleFactor, GetRelativeDimensions().y * scaleFactor)); }

	void SetParent(GUIComponent *parent) { mParent = parent; }
	bool IsInside(float x, float y);

	void SetVisible(bool visible) { mIsVisible = visible; }
	bool IsVisible() const { return mIsVisible; }

	void SetFocused(bool focused) { mIsFocused = focused; }
	bool IsFocused() const { return mIsFocused; }

	void SetSelected(bool selected) { mIsSelected = selected; }
	bool IsSelected() const { return mIsSelected; }

	void SetName(const std::string &name) { mName = name; }
	std::string GetName() const { return mName; }

	virtual void OnEntry() {}
	virtual void OnExit() {}
	virtual void OnMouseEnter() {}
	virtual void OnMouseLeave() {}
	virtual void OnMousePress() {}
	virtual void OnMouseRelease() {}
	virtual void OnMouseMove(int dx, int dy) {}
	virtual void OnKeyPress(int key) {}

protected:
	void Update_();
private:
	GUIComponent *mParent;

	XMFLOAT2 mRelativePositionTL;
	XMFLOAT2 mNormalizedPositionTL;
	XMFLOAT2 mScreenPositionTL;

	XMFLOAT2 mRelativeDimensions;
	XMFLOAT2 mNormalizedDimensions;	
	XMFLOAT2 mScreenDimensions;

	bool mIsVisible;
	bool mIsFocused = false;
	bool mIsSelected = false;

	std::string mName;
};

#endif  // GUICOMPONENT_H