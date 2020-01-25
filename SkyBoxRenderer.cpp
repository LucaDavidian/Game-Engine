#include "SkyBoxRenderer.h"
#include "Entity.h"
#include "PositionComponent.h"
#include "CameraComponent.h"
#include "StaticMeshComponent.h"

void SkyBoxRenderer::Render(Entity *skyBox, Entity *camera)
{
	mShader.Use();

	XMFLOAT4X4 worldMatrix = skyBox->GetComponent<PositionComponent>()->GetWorldMatrix();
	XMFLOAT4X4 viewMatrix = camera->GetComponent<CameraComponent>()->GetViewMatrix();
	XMFLOAT4X4 projectionMatrix = camera->GetComponent<CameraComponent>()->GetProjectionMatrix();

	viewMatrix._41 = 0.0f;
	viewMatrix._42 = 0.0f;
	viewMatrix._43 = 0.0f;

	XMFLOAT4X4 worldViewProjectionMatrix;
	XMStoreFloat4x4(&worldViewProjectionMatrix, XMMatrixMultiply(XMMatrixMultiply(XMLoadFloat4x4(&worldMatrix), XMLoadFloat4x4(&viewMatrix)), XMLoadFloat4x4(&projectionMatrix)));
	mShader.UpdateTransformConstantBuffer(worldViewProjectionMatrix);

	skyBox->GetComponent<StaticMeshComponent>()->GetMaterials()[0].GetDiffuseMaps()[0].Bind(0);

	skyBox->GetComponent<StaticMeshComponent>()->GetMeshes()[0].Bind();
	skyBox->GetComponent<StaticMeshComponent>()->GetMeshes()[0].Draw();
}