#ifndef GUITEXTFIELD_H
#define GUITEXTFIELD_H

#include "GUIComponent.h"
#include "Text.h"

class GUITextField : public GUIComponent
{
public:
	GUITextField() : mText(new Text(10)) {}
	~GUITextField() { delete mText; }

	void AcceptPreorder(GUIVisitor &visitor) { visitor.Visit(this); }
	void AcceptPostorder(GUIVisitor &visitor) { visitor.Visit(this); }

	void SetTextField(const std::string &text) { mText->SetText(text); }
	void AppendTextField(const std::string &text) { mText->AppendText(text); }
	std::string const GetTextField() const { return mText->GetText(); }

	void SetTextColor(const XMFLOAT3 &color) { mText->SetColor(color); }

	void SetBackgroundColor(const XMFLOAT3 &color) { mColor = color; }
	XMFLOAT3 GetBackgroundColor() const { return mColor; }

	void SetOpacity(float opacity) { mOpacity = opacity; }
	float GetOpacity() const { return mOpacity; }

	Text *GetText() const { return mText; }

	void Update() override;

	void OnMouseEnter() override { SetFocused(true);  SetBackgroundColor(mHoverColor); }
	void OnMouseLeave() override { SetFocused(false); SetBackgroundColor(mIdleColor); }
	void OnMousePress() override;
	void OnKeyPress(int key) override;
private:
	XMFLOAT3 mColor;
	float mOpacity = 1.0f;
	XMFLOAT3 mHoverColor = XMFLOAT3(1.0f, 0.0f, 0.0f);
	XMFLOAT3 mIdleColor = XMFLOAT3(0.0, 0.0, 1.0);

	Text *mText;
};

#endif  // GUITEXTFIELD_H

