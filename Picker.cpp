#include "Picker.h"
#include "GraphicsSystem.h"
#include "CollisionSystem.h"

void Picker::CreateRay(int x, int y)
{
	unsigned displayWidth = GraphicsSystem::GetInstance().GetDisplayWidth();
	unsigned displayHeight = GraphicsSystem::GetInstance().GetDisplayHeight();

	GraphicsSystem::Viewport viewport = GraphicsSystem::GetInstance().GetViewport();

	int viewportX = viewport.x;
	int viewportY = viewport.y;
	int viewportWidth = viewport.width;
	int viewPortHeight = viewport.height;

	// inverse viewport transformation
	float ndcX = (x - viewportX) * 2.0f / viewportWidth - 1.0f;  
	float ndcY = (-y + viewportY) * 2.0f / viewPortHeight + 1.0f;

	float cameraNear = mCamera->GetNearDistance();
	float cameraFar = mCamera->GetFarDistance();

	// point on projection plane (homogeneous clip space - projection space)
	XMFLOAT4 projectedPointNDC(ndcX * cameraNear, ndcY * cameraNear, 0.0f * cameraNear, cameraNear);

	// point in view space
	XMFLOAT4X4 projectionMatrix = mCamera->GetProjectionMatrix();
	XMMATRIX inverseProjectionMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&projectionMatrix));
	XMVECTOR projectedPointView = XMVector4Transform(XMLoadFloat4(&projectedPointNDC), inverseProjectionMatrix);

	// ray in world space
	XMFLOAT4X4 viewMatrix = mCamera->GetViewMatrix();
	XMMATRIX inverseViewMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&viewMatrix));
	XMVECTOR rayView = XMVector3Normalize(projectedPointView);
	XMVECTOR rayWorld = XMVector3TransformNormal(rayView, inverseViewMatrix);

	XMStoreFloat3(&mRay, rayWorld);

	CollisionSystem::GetInstance().AddRay(this);
}
