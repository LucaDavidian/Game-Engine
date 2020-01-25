#include "QuadRenderer.h"
#include "Texture.h"

QuadRenderer::QuadRenderer()
{
	XMFLOAT2 positions[] = { XMFLOAT2(-0.5f, 0.5f), XMFLOAT2(0.5f, 0.5f), XMFLOAT2(-0.5f, -0.5f), XMFLOAT2(0.5f, -0.5f) };
	XMFLOAT2 texCoords[] = { XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f) };

	mMesh.LoadAttribute("POSITION", positions, 4);
	mMesh.LoadAttribute("TEX_COORD", texCoords, 4);
	mMesh.SetVertexCount(4);
}

void QuadRenderer::AddQuad(const XMFLOAT2 &position, const XMFLOAT2 &dimensions, const XMFLOAT3 &color, float opacity, Texture *texture)
{
	mQuads.emplace_back(position, dimensions, color, opacity, texture);
}

void QuadRenderer::Render()
{
	mShader.Use();

	for (Quad &quad : mQuads)
	{
		XMFLOAT2 position = quad.mPosition;
		XMFLOAT2 scale = quad.mDimensions;
		
		XMMATRIX scaling = XMMatrixScaling(scale.x, scale.y, 1.0f);
		
		XMMATRIX translation = XMMatrixTranslation(position.x, position.y, 0.0f);

		XMFLOAT4X4 worldMatrix;
		XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(scaling, translation));

		mShader.UpdateTransformConstantBuffer(worldMatrix);

		if (!quad.mTexture)
			mShader.UpdateMaterialConstantBuffer(false, quad.mColor, quad.mOpacity);
		else
		{
			mShader.UpdateMaterialConstantBuffer(true, quad.mColor, quad.mOpacity);
			quad.mTexture->Bind(0);
		}	

		GraphicsSystem::GetInstance().SetBlendState(GraphicsSystem::BlendState::ADDITIVE);
		mMesh.Bind();
		mMesh.Draw();
		GraphicsSystem::GetInstance().SetBlendState(GraphicsSystem::BlendState::DISABLED);
	}

	mQuads.clear();
}