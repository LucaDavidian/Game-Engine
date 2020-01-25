#ifndef GUILABEL_H
#define GUILABEL_H

#include "GUIComponent.h"
#include "Text.h"

class GUILabel : public GUIComponent
{
public:
	GUILabel(const std::string &labelText);
	~GUILabel() { delete mText; }

	void AcceptPreorder(GUIVisitor &visitor) { visitor.Visit(this); }
	void AcceptPostorder(GUIVisitor &visitor) { visitor.Visit(this); }

	void SetLabel(const std::string &text) { mText->SetText(text); }
	std::string const GetLabel() const { return mText->GetText(); }

	void SetColor(const XMFLOAT3 &color) { mColor = color; }
	XMFLOAT3 GetColor() const { return mColor; }

	void SetOpacity(float opacity) { mOpacity = opacity; }
	float GetOpacity() const { return mOpacity; }

	void SetTexture(Texture *texture) { mTexture = texture; }
	Texture *GetTexture() const { return mTexture; }	

	Text *GetText() const { return mText; }

	void Update() override;
private:
	XMFLOAT3 mColor;
	float mOpacity = 0.0f;

	Texture *mTexture;

	Text *mText;
};

#endif  // GUILABEL_H

