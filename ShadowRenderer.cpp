#include "ShadowRenderer.h"
#include "Entity.h"
#include "PositionComponent.h"
#include "StaticMeshComponent.h"
#include "LightComponent.h"
//#include "SkeletalMeshComponent.h"
#include "CameraComponent.h"

ShadowRenderer::ShadowRenderer(int width, int height, float shadowDistance)
	: mFrameBuffer(width, height), mFrameBufferSpot(width, height), mShadowDistance(shadowDistance)
{
	mShadowMap.SetResourceView(mFrameBuffer.GetDepthStencilBuffer());
	mShadowMapSpot.SetResourceView(mFrameBufferSpot.GetDepthStencilBuffer());
}

void ShadowRenderer::ComputeLightViewProjection(Entity *camera, Entity *light)
{
	if (light->GetComponent<LightComponent>()->GetType() == LightComponent::Type::DIRECTIONAL)
	{
		CameraComponent::Frustum frustum = camera->GetComponent<CameraComponent>()->GetFrustum(mShadowDistance);
		XMFLOAT3 lightDirection = light->GetComponent<PositionComponent>()->GetAxisZ();
		XMFLOAT3 lightPosition = light->GetComponent<PositionComponent>()->GetPosition();

		XMMATRIX inverseCameraViewMatrix = XMLoadFloat4x4(&camera->GetComponent<CameraComponent>()->GetInverseViewMatrix());
		XMMATRIX lightViewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&lightPosition), XMLoadFloat3(&lightDirection), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		XMMATRIX transform = XMMatrixMultiply(inverseCameraViewMatrix, lightViewMatrix);

		// view frustum vertices in light space coordinates
		XMFLOAT3 frustumVertices[8];
		XMStoreFloat3(&frustumVertices[0], XMVector3TransformCoord(XMLoadFloat3(&frustum.nearTopLeft), transform));
		XMStoreFloat3(&frustumVertices[1], XMVector3TransformCoord(XMLoadFloat3(&frustum.nearTopRight), transform));
		XMStoreFloat3(&frustumVertices[2], XMVector3TransformCoord(XMLoadFloat3(&frustum.nearBottomLeft), transform));
		XMStoreFloat3(&frustumVertices[3], XMVector3TransformCoord(XMLoadFloat3(&frustum.nearBottomRight), transform));
		XMStoreFloat3(&frustumVertices[4], XMVector3TransformCoord(XMLoadFloat3(&frustum.farTopLeft), transform));
		XMStoreFloat3(&frustumVertices[5], XMVector3TransformCoord(XMLoadFloat3(&frustum.farTopRight), transform));
		XMStoreFloat3(&frustumVertices[6], XMVector3TransformCoord(XMLoadFloat3(&frustum.farBottomLeft), transform));
		XMStoreFloat3(&frustumVertices[7], XMVector3TransformCoord(XMLoadFloat3(&frustum.farBottomRight), transform));

		float xMin = frustumVertices[0].x, xMax = xMin;
		float yMin = frustumVertices[0].y, yMax = yMin;
		float zMin = frustumVertices[0].z, zMax = zMin;

		for (XMFLOAT3 frustumVertex : frustumVertices)
		{
			if (frustumVertex.x < xMin)
				xMin = frustumVertex.x;
			if (frustumVertex.x > xMax)
				xMax = frustumVertex.x;
			if (frustumVertex.y < yMin)
				yMin = frustumVertex.y;
			if (frustumVertex.y > yMax)
				yMax = frustumVertex.y;
			if (frustumVertex.z < zMin)
				zMin = frustumVertex.z;
			if (frustumVertex.z > zMax)
				zMax = frustumVertex.z;
		}

		// calculate width, height and depth of view cuboid
		float width = xMax - xMin;
		float height = yMax - yMin;
		float depth = zMax - zMin;

		// calculate light's position in light space coordinates
		float x = (xMin + xMax) / 2.0f;
		float y = (yMin + yMax) / 2.0f;
		float z = zMin - 0.1f;

		// light's position in world space coordinates
		XMVECTOR viewPosition = XMVectorSet(x, y, z, 1.0f);
		XMVECTOR worldViewPosition = XMVector3TransformCoord(viewPosition, XMMatrixInverse(nullptr, lightViewMatrix));
		XMMATRIX viewMatrix = XMMatrixLookAtLH(worldViewPosition, worldViewPosition + XMLoadFloat3(&lightDirection), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		XMMATRIX projectionMatrix = XMMatrixOrthographicLH(width, height, 0.1f, 0.1f + depth);

		XMStoreFloat4x4(&mLightViewProjectionMatrix, XMMatrixMultiply(viewMatrix, projectionMatrix));
	}

	if (light->GetComponent<LightComponent>()->GetType() == LightComponent::Type::SPOT)
	{
		PositionComponent *positionComponent = light->GetComponent<PositionComponent>();

		XMFLOAT3 lightPosition = positionComponent->GetPosition();
		XMFLOAT3 lightDirection = positionComponent->GetAxisZ();

		XMMATRIX lightViewMatrix = XMMatrixLookAtLH(XMLoadFloat3(&lightPosition), XMLoadFloat3(&lightPosition) + XMLoadFloat3(&lightDirection), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));
		XMMATRIX lightProjectionMatrix = XMMatrixPerspectiveFovLH(XMConvertToRadians(90.0f), 3.0f / 2.0f, 0.1f, 500.0f);

		XMStoreFloat4x4(&mLightViewProjectionMatrixSpot, XMMatrixMultiply(lightViewMatrix, lightProjectionMatrix));
	}
}

void ShadowRenderer::Render(Entity *camera, Entity *light, Entity *spotlight)
{
	ComputeLightViewProjection(camera, light);  // call from rendering system (multiple light sources)
	ComputeLightViewProjection(camera, spotlight);

	Vector<Entity*> staticEntities;
	Vector<Entity*> animatedEntities;

	for (Entity *entity : mEntities)
		if (entity->HasComponent<StaticMeshComponent>())
			staticEntities.InsertLast(entity);
		else
			animatedEntities.InsertLast(entity);

	// render static entities' shadows
	mShader.Use();

	mFrameBuffer.Set(true);

	for (Entity *entity : staticEntities)
	{
		StaticMeshComponent *staticMeshComponent = entity->GetComponent<StaticMeshComponent>();

		mShader.UpdateTransformConstantBuffer(entity->GetComponent<PositionComponent>()->GetWorldMatrixScale(), mLightViewProjectionMatrix);

		Mesh mesh = staticMeshComponent->GetMeshes()[0];
		mesh.BindAttribute("POSITION", 0);  // bind just position vertex attribute
		mesh.Draw();
	}

	mFrameBuffer.Unset();

	mFrameBufferSpot.Set(true);

	for (Entity *entity : staticEntities)
	{
		StaticMeshComponent *staticMeshComponent = entity->GetComponent<StaticMeshComponent>();

		mShader.UpdateTransformConstantBuffer(entity->GetComponent<PositionComponent>()->GetWorldMatrixScale(), mLightViewProjectionMatrixSpot);

		Mesh mesh = staticMeshComponent->GetMeshes()[0];
		mesh.BindAttribute("POSITION", 0);  // bind just position vertex attribute
		mesh.Draw();
	}

	mFrameBufferSpot.Unset();

	// render animated entities' shadows
	/*mAnimationShader.Use();

	for (Entity *entity : animatedEntities)
	{
		SkeletalMeshComponent *skeletalMeshComponent = entity->GetComponents<SkeletalMeshComponent>()[0];

		mAnimationShader.UpdateTransformConstantBuffer(entity->GetComponents<PositionComponent>()[0]->GetWorldMatrix(), mLightViewProjectionMatrix);
		mAnimationShader.UpdateBoneTransformConstantBuffer(skeletalMeshComponent->GetSkeleton()->GetBoneTransformMatrices());

		for (Mesh *mesh : skeletalMeshComponent->GetMeshes())
		{
			mesh->BindAttribute("POSITION", 0);
			mesh->BindAttribute("BONE_IDS", 1);
			mesh->BindAttribute("BONE_WEIGHTS", 2);
			mesh->BindIndexBuffer();
			mesh->Draw();
		}		
	}*/

	mEntities.Clear();
}
