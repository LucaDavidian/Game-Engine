#include "TextRenderer.h"
#include "Text.h"

void TextRenderer::AddText(Text *text)
{
	mTexts.push_back(text);
}

void TextRenderer::Render()
{
	mShader.Use();

	for (Text *text : mTexts)
	{
		XMFLOAT4X4 worldMatrix;
		XMFLOAT2 position = text->GetNormalizedPosition();
		float scale = text->GetScale();
		
		XMMATRIX scaling = XMMatrixScaling(scale, scale, 1.0f);
		XMMATRIX translation = XMMatrixTranslation(position.x, position.y, 0.0f);
		XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(scaling, translation));

		mShader.UpdateTransformConstantBuffer(worldMatrix);

		mShader.UpdateTextParamsConstantBuffer(text->GetColor());

		text->GetFont()->GetAtlas().Bind(0);
		
		GraphicsSystem::GetInstance().SetBlendState(GraphicsSystem::BlendState::ADDITIVE);
		text->GetMesh().Bind();
		text->GetMesh().Draw();
		GraphicsSystem::GetInstance().SetBlendState(GraphicsSystem::BlendState::DISABLED);
	}

	mTexts.clear();
}