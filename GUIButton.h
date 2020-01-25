#ifndef GUIBUTTON_H
#define GUIBUTTON_H

#include "GUIComposite.h"
#include "TextureManager.h"

#ifdef FAST_DELEGATES
#	include "fast delegates/delegate.hpp"
#else
	#include "delegates/delegate.hpp"
#endif  // FAST_DELEGATES

#include "EventSystem.h"

class Texture;

class GUIButton : public GUIComposite
{
private:
	MULTICAST_DELEGATE_ONE_PARAM(ButtonListener, const std::string &);
public:
	GUIButton(const XMFLOAT3 &color, const XMFLOAT3 &highlight, const XMFLOAT3 &selectedColor) : mBaseColor(color), mHighlightColor(highlight), mSelectedColor(selectedColor), mTexture(nullptr) { SetColor(color); }

	void AcceptPreorder(GUIVisitor &visitor) override { visitor.Visit(this); GUIComposite::AcceptPreorder(visitor); }
	void AcceptPostorder(GUIVisitor &visitor) override { GUIComposite::AcceptPostorder(visitor); visitor.Visit(this); }

	void SetColor(const XMFLOAT3 &color) { mColor = color; }
	XMFLOAT3 GetColor() const { return mColor; }

	void SetOpacity(float opacity) { mOpacity = opacity; }
	float GetOpacity() const { return mOpacity; }

	void SetTexture(Texture *texture) { mTexture = texture; }
	Texture *GetTexture() const { return mTexture; }

	virtual void OnMouseEnter() override;
	virtual void OnMouseLeave() override;
	virtual void OnMousePress() override; 
	virtual void OnMouseRelease() override; 

	template <typename T>
	void AddButtonListener(T &instance, void(T::*ptr)(const std::string &)) { mButtonListeners.Bind(instance, ptr); }
private:
	XMFLOAT3 mColor;
	float mOpacity = 1.0f;
	XMFLOAT3 mBaseColor = XMFLOAT3(0.6f, 0.6f, 0.6f);
	XMFLOAT3 mHighlightColor = XMFLOAT3(1.0f, 1.0f, 0.0f);
	XMFLOAT3 mSelectedColor = XMFLOAT3(1.0f, 0.0f, 0.0f);

	Texture *mTexture;
	Texture *mActiveTexture = &TextureManager::GetInstance().GetTexture("res/action.png");
	Texture *mUnactiveTexture = &TextureManager::GetInstance().GetTexture("res/button.png");

	ButtonListener mButtonListeners;
};

#endif  // GUIBUTTON_H

