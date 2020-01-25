#include "GUIButton.h"

void GUIButton::OnMouseEnter() 
{ 
	if (!IsFocused() && !IsSelected())
		SetColor(mHighlightColor);
	else if (!IsFocused() && IsSelected())
		SetColor(mSelectedColor);

	Scale(1.1f);
	SetFocused(true);
}

void GUIButton::OnMouseLeave() 
{ 
	if (IsFocused() && !IsSelected())
		SetColor(mBaseColor);

	else if (IsFocused() && IsSelected())
		SetColor(mHighlightColor);

	Scale(1 / 1.1f);
	SetFocused(false);  
}

void GUIButton::OnMousePress() 
{ 
	if (IsFocused() && !IsSelected()) 
	{ 
		SetColor(mSelectedColor); 
		SetSelected(true); 
	} 
}

void GUIButton::OnMouseRelease() 
{ 
	if (IsFocused() && IsSelected()) 
	{ 
		Scale(1 / 1.1f);
		SetColor(mHighlightColor); 
		mButtonListeners.Invoke(GetName());
	} 
	else if (!IsFocused() && IsSelected())
		SetColor(mBaseColor);

	SetSelected(false); 
}
