#ifndef GUIPANEL_H
#define GUIPANEL_H

#include "GUIComposite.h"

class Texture;

class GUIPanel : public GUIComposite
{
public:
	GUIPanel() : mTexture(nullptr) { SetColor(XMFLOAT3(0.7f, 0.7f, 0.7f)); }

	void AcceptPreorder(GUIVisitor &visitor) override { visitor.Visit(this); GUIComposite::AcceptPreorder(visitor); }
	void AcceptPostorder(GUIVisitor &visitor) override { GUIComposite::AcceptPostorder(visitor); visitor.Visit(this); }

	void SetColor(const XMFLOAT3 &color) { mColor = color; }
	XMFLOAT3 GetColor() const { return mColor; }

	void SetOpacity(float opacity) { mOpacity = opacity; }
	float GetOpacity() const { return mOpacity; }

	void SetTexture(Texture *texture) { mTexture = texture; }
	Texture *GetTexture() const { return mTexture; }

	virtual void OnEntry() override {}
	virtual void OnExit() override {}
	virtual void OnMouseEnter() override { SetFocused(true); }
	virtual void OnMouseLeave() override { SetFocused(false); }
	virtual void OnMousePress() override { if (IsFocused()) SetSelected(true);  }
	virtual void OnMouseRelease() override { SetSelected(false); }
	virtual void OnMouseMove(int dx, int dy) override { if (IsSelected()) SetScreenPositionTL(XMFLOAT2(GetScreenPositionTL().x + dx, GetScreenPositionTL().y + dy));
	}

private:
	XMFLOAT3 mColor;
	float mOpacity = 1.0f;

	Texture *mTexture;
};

#endif  // GUIPANEL_H
