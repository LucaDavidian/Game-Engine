#include "StaticEntityRenderer.h"
#include "Entity.h"
#include "Texture.h"
#include "PositionComponent.h"
#include "StaticMeshComponent.h"
#include "CameraComponent.h"
#include "Game.h"

void StaticEntityRenderer::Render(Entity *camera, Vector<Entity*> const &lights, Texture shadowMap, Texture shadowMapSpot, XMFLOAT4X4 const &lightViewProjectionMatrix, XMFLOAT4X4 const &lightViewProjectionMatrixSpot, float shadowDistance)
{
	mShader.Use();

	mShader.UpdateCameraConstantBuffer(camera->GetComponent<CameraComponent>()->GetPosition(), shadowDistance);
	mShader.UpdateLightConstantBuffer(lights);

	XMFLOAT4X4 viewMatrix = camera->GetComponent<CameraComponent>()->GetViewMatrix();
	XMFLOAT4X4 projectionMatrix = camera->GetComponent<CameraComponent>()->GetProjectionMatrix();

	shadowMap.Bind(0);
	shadowMapSpot.Bind(4);

	for (Entity *entity : mEntities)
	{
		PositionComponent *positionComponent = entity->GetComponent<PositionComponent>();
		StaticMeshComponent *staticMeshComponent = entity->GetComponent<StaticMeshComponent>();

		XMFLOAT4X4 worldMatrix = positionComponent->GetWorldMatrixScale();

		XMFLOAT4X4 worldInverseTransposeMatrix;
		XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixTranspose(XMMatrixInverse(nullptr, XMLoadFloat4x4(&worldMatrix))));

		XMFLOAT4X4 worldViewProjectionMatrix;
		XMStoreFloat4x4(&worldViewProjectionMatrix, XMMatrixMultiply(XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&viewMatrix)), XMLoadFloat4x4(&projectionMatrix)));

		mShader.UpdateTransformConstantBuffer(worldMatrix, worldInverseTransposeMatrix, worldViewProjectionMatrix, lightViewProjectionMatrix, lightViewProjectionMatrixSpot);

		Material material = staticMeshComponent->GetMaterials()[0];
		mShader.UpdateMaterialConstantBuffer(material);

		Mesh mesh = staticMeshComponent->GetMeshes()[0];

		if (staticMeshComponent->GetMaterials()[0].HasDiffuseMap())
		{
			Texture texture = staticMeshComponent->GetMaterials()[0].GetDiffuseMaps()[0];
			texture.Bind(1);
		}

		if (staticMeshComponent->GetMaterials()[0].HasSpecularMap())
		{
			Texture texture = staticMeshComponent->GetMaterials()[0].GetSpecularMaps()[0];
			texture.Bind(2);
		}

		if (staticMeshComponent->GetMaterials()[0].HasNormalMap())
		{
			Texture texture = staticMeshComponent->GetMaterials()[0].GetNormalMaps()[0];
			texture.Bind(3);
		}
	
		// bind and draw mesh
		mesh.Bind();
		mesh.Draw();
	}

	shadowMap.Unbind();
	shadowMapSpot.Unbind();

	// clear entities queue
	mEntities.Clear();
}