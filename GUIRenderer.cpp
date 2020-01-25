#include "GUIRenderer.h"

#include "GUISystem.h"
#include "GUIPanel.h"
#include "GUIButton.h"
#include "GUILabel.h"
#include "GUITextField.h"

#include "QuadRenderer.h"
#include "TextRenderer.h"

void GUIRenderer::RenderGUIVisitor::Visit(GUIPanel *component)
{ 
	QuadRenderer::GetInstance().AddQuad(component->GetNormalizedPositionCenter(), component->GetNormalizedDimensions(), component->GetColor(), component->GetOpacity(), component->GetTexture());
}

void GUIRenderer::RenderGUIVisitor::Visit(GUIButton *component)
{
	QuadRenderer::GetInstance().AddQuad(component->GetNormalizedPositionCenter(), component->GetNormalizedDimensions(), component->GetColor(), component->GetOpacity(), component->GetTexture());
}

void GUIRenderer::RenderGUIVisitor::Visit(GUILabel *component)
{
	QuadRenderer::GetInstance().AddQuad(component->GetNormalizedPositionCenter(), component->GetNormalizedDimensions(), component->GetColor(), component->GetOpacity(), nullptr);
	TextRenderer::GetInstance().AddText(component->GetText());
}

void GUIRenderer::RenderGUIVisitor::Visit(GUITextField *component)
{
	QuadRenderer::GetInstance().AddQuad(component->GetNormalizedPositionCenter(), component->GetNormalizedDimensions(), component->GetBackgroundColor(), component->GetOpacity(), nullptr);
	TextRenderer::GetInstance().AddText(component->GetText());
}

void GUIRenderer::Render() 
{ 
	GraphicsSystem::GetInstance().SetDepthStencilState(GraphicsSystem::DepthStencilState::DISABLED); 
	
	RenderGUIVisitor visitor;

	for (auto it = GUISystem::GetInstance().GetGUIs()->begin(); it != GUISystem::GetInstance().GetGUIs()->end(); ++it)
	{
		if (!(*it)->IsVisible())
			continue;

		(*it)->GetRoot()->AcceptPreorder(visitor);

		QuadRenderer::GetInstance().Render();
		TextRenderer::GetInstance().Render();
	}

	GraphicsSystem::GetInstance().SetDepthStencilState(GraphicsSystem::DepthStencilState::ENABLED);
}
