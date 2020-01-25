#include "CollisionComponent.h"
#include "Entity.h"
#include "PositionComponent.h"

CollisionComponent::CollisionComponent(Type type, const XMFLOAT3 &relativePosition) : mType(type), mRelativePosition(relativePosition)
{
	XMStoreFloat4x4(&mOffsetMatrix, XMMatrixIdentity());

	mOffsetMatrix._41 = mRelativePosition.x;
	mOffsetMatrix._42 = mRelativePosition.y;
	mOffsetMatrix._43 = mRelativePosition.z;
}

XMFLOAT3 const CollisionComponent::GetPosition() const
{
	Entity *owner = GetOwner();
	PositionComponent *positionComponent = owner->GetComponent<PositionComponent>();

	XMFLOAT4X4 worldMatrix = positionComponent->GetWorldMatrix();

	XMFLOAT3 position;
	XMStoreFloat3(&position, XMVector3Transform(XMLoadFloat3(&mRelativePosition), XMLoadFloat4x4(&worldMatrix)));

	return position;
}

XMFLOAT4X4 const CollisionComponent::GetWorldMatrix() const
{
	Entity *owner = GetOwner();
	PositionComponent *positionComponent = owner->GetComponent<PositionComponent>();

	XMFLOAT4X4 entityWorldMatrix = positionComponent->GetWorldMatrix();

	XMFLOAT4X4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMLoadFloat4x4(&mOffsetMatrix), XMLoadFloat4x4(&entityWorldMatrix)));

	return worldMatrix;
}

XMFLOAT4X4 const CollisionComponent::GetInverseWorldMatrix() const
{
	XMFLOAT4X4 worldMatrix = GetWorldMatrix();

	XMFLOAT4X4 inverseWorldMatrix;
	XMStoreFloat4x4(&inverseWorldMatrix, XMMatrixInverse(nullptr, XMLoadFloat4x4(&worldMatrix)));

	return inverseWorldMatrix;
}

const XMFLOAT3 CollisionComponent::GetAxis(int axis) const
{ 
	PositionComponent *positionComponent = GetOwner()->GetComponent<PositionComponent>();    // TODO: when relative orientation is introduced calculate axis

	if (axis == 0)
		return positionComponent->GetAxisX();
	else if (axis == 1)
		return positionComponent->GetAxisY();
	else // axis == 2
		return positionComponent->GetAxisZ();
}

