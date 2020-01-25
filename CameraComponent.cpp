#include "CameraComponent.h"
#include "PositionComponent.h"

CameraComponent::CameraComponent(float verticalFOV, float aspectRatio, float nearDistance, float farDistance, PositionComponent *positionComponent, const XMFLOAT3 &relativePosition, const XMFLOAT3 &relativeOrientationEulerAngles)
	: mPositionComponent(positionComponent), mRelativePosition(relativePosition), mRelativeOrientationEulerAngles(relativeOrientationEulerAngles)
{
	// set perspective projection matrix
	SetLens(verticalFOV, aspectRatio, nearDistance, farDistance);

	// calculate offset matrix
	XMMATRIX relativeRotationMatrix = XMMatrixRotationRollPitchYaw(mRelativeOrientationEulerAngles.x, mRelativeOrientationEulerAngles.y, mRelativeOrientationEulerAngles.z);
	XMMATRIX relativeTranslationMatrix = XMMatrixTranslation(mRelativePosition.x, mRelativePosition.y, mRelativePosition.z);
	
	XMMATRIX offsetMatrix = XMMatrixInverse(nullptr, XMMatrixMultiply(relativeRotationMatrix, relativeTranslationMatrix));

	XMStoreFloat4x4(&mOffsetMatrix, offsetMatrix);
}

void CameraComponent::SetLens(float verticalFOV, float aspectRatio, float nearDistance, float farDistance)
{
	// set camera frustum parameters
	mVerticalFOV = verticalFOV;
	mAspectRatio = aspectRatio;
	mNearDistance = nearDistance;
	mFarDistance = farDistance;

	// update perspective projection matrix
	XMStoreFloat4x4(&mProjectionMatrix, XMMatrixPerspectiveFovLH(mVerticalFOV, mAspectRatio, mNearDistance, mFarDistance));
}

const XMFLOAT3 CameraComponent::GetPosition() const
{
	XMFLOAT3 targetPosition = mPositionComponent->GetPosition();
	XMVECTOR relativePosition = XMVector3Transform(XMLoadFloat3(&mRelativePosition), XMLoadFloat4x4(&mPositionComponent->GetWorldMatrix()));

	XMFLOAT3 cameraPosition;
	XMStoreFloat3(&cameraPosition, relativePosition);

	return cameraPosition;
}

const XMFLOAT4X4 CameraComponent::GetViewMatrix() const
{
	XMMATRIX targetInverseWorldMatrix = XMLoadFloat4x4(&mPositionComponent->GetInverseWorldMatrix());
	XMMATRIX offsetMatrix = XMLoadFloat4x4(&mOffsetMatrix);

	XMFLOAT4X4 viewMatrix;
	XMStoreFloat4x4(&viewMatrix, XMMatrixMultiply(targetInverseWorldMatrix, offsetMatrix));

	return viewMatrix;
}

const XMFLOAT4X4 CameraComponent::GetInverseViewMatrix() const
{
	XMMATRIX targetWorldMatrix = XMLoadFloat4x4(&mPositionComponent->GetWorldMatrix());
	XMMATRIX inverseOffsetMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&mOffsetMatrix));

	XMFLOAT4X4 inverseViewMatrix;
	XMStoreFloat4x4(&inverseViewMatrix, XMMatrixMultiply(inverseOffsetMatrix, targetWorldMatrix));

	return inverseViewMatrix;
}

// parameter defines the frustum depth (1.0 = real frustum depth)
CameraComponent::Frustum const CameraComponent::GetFrustum(float maxDistance) const
{
	Frustum frustum;

	float nearHeight = 2.0f * mNearDistance * tan(mVerticalFOV / 2.0f);
	float nearWidth = mAspectRatio * nearHeight;
	float farHeight = 2.0f * mFarDistance * tan(mVerticalFOV / 2.0f);
	float farWidth = mAspectRatio * farHeight;
	float farDistance = mFarDistance;

	if (maxDistance < mFarDistance)
	{
		farDistance = maxDistance;
		farHeight = 2.0f * farDistance * tan(mVerticalFOV / 2.0f);
		farWidth = mAspectRatio * farHeight;
	}

	frustum.nearTopLeft = XMFLOAT3(-nearWidth / 2.0f, nearHeight / 2.0f, mNearDistance);
	frustum.nearTopRight = XMFLOAT3(nearWidth / 2.0f, nearHeight / 2.0f, mNearDistance);
	frustum.nearBottomLeft = XMFLOAT3(-nearWidth / 2.0f, -nearHeight / 2.0f, mNearDistance);
	frustum.nearBottomRight = XMFLOAT3(nearWidth / 2.0f, -nearHeight / 2.0f, mNearDistance);
	frustum.farTopLeft = XMFLOAT3(-farWidth / 2.0f, farHeight / 2.0f, farDistance);
	frustum.farTopRight = XMFLOAT3(farWidth / 2.0f, farHeight / 2.0f, farDistance);
	frustum.farBottomLeft = XMFLOAT3(-farWidth / 2.0f, -farHeight / 2.0f, farDistance);
	frustum.farBottomRight = XMFLOAT3(farWidth / 2.0f, -farHeight / 2.0f, farDistance);

	return frustum;
}