#include "TerrainRenderer.h"
#include "Terrain.h"
#include "Entity.h"
#include "Texture.h"

void TerrainRenderer::Render(Entity *camera, Vector<Entity*> const &lights, Texture shadowMap, Texture shadowMapSpot, XMFLOAT4X4 const &lightViewProjectionMatrix, XMFLOAT4X4 const &lightViewProjectionMatrixSpot, float shadowDistance, const XMFLOAT4 &clipPlane)
{
	mShader.Use();

	mShader.UpdateLightConstantBuffer(lights);

	mShader.UpdateClipPlaneConstantBuffer(clipPlane);

	mShader.UpdateCameraConstantBuffer(camera->GetPosition(), shadowDistance);  

	XMFLOAT4X4 viewMatrix = camera->GetViewMatrix();
	XMFLOAT4X4 projectionMatrix = camera->GetProjectionMatrix();

	for (Terrain *terrain : mTerrains)
	{
		XMFLOAT4X4 worldMatrix;
		XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());

		XMFLOAT4X4 worldViewProjectionMatrix;
		XMStoreFloat4x4(&worldViewProjectionMatrix, XMMatrixMultiply(XMLoadFloat4x4(&viewMatrix), XMLoadFloat4x4(&projectionMatrix)));

		mShader.UpdateTransformConstantBuffer(worldMatrix, worldViewProjectionMatrix, lightViewProjectionMatrix);

		std::vector<Texture*> textures = terrain->GetModel()->GetTextures()[0];

		int i = 0;

		depthMap->Bind(i++);

		for (Texture *texture : textures)
			texture->Bind(i++);
			
		terrain->GetModel()->GetMeshes()[0]->Bind();
		terrain->GetModel()->GetMeshes()[0]->Draw();
	}
}
