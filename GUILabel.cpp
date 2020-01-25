#include "GUILabel.h"

GUILabel::GUILabel(const std::string &labelText) : mText(new Text(labelText.size())) 
{
	mText->SetText(labelText);
	mText->SetFont("filmgoer.otf", 48);
	mText->SetNormalizedTextBoxDimensions(GetNormalizedDimensions());
}

void GUILabel::Update()
{
	GUIComponent::Update();

	mText->SetNormalizedPositionCenter(GetNormalizedPositionCenter());
}