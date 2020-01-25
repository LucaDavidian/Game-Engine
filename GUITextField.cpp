#include "GUITextField.h"

void GUITextField::Update()
{
	GUIComponent::Update();

	mText->SetNormalizedPositionTL(GetNormalizedPositionTL());
	mText->SetNormalizedTextBoxDimensions(GetNormalizedDimensions());
}

void GUITextField::OnMousePress()
{
	if (IsFocused())
		SetSelected(true);
	else
		SetSelected(false);
}

void GUITextField::OnKeyPress(int key)
{
	mText->AppendChar(key);

	mText->SetNormalizedPositionTL(GetNormalizedPositionTL());
}