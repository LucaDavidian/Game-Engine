#include "PlaneCollisionComponent.h"

PlaneCollisionComponent::PlaneCollisionComponent(const XMFLOAT3 &normal, const XMFLOAT3 &relativePosition) : CollisionComponent(Type::PLANE, relativePosition)
{
	XMStoreFloat3(&mNormal, XMVector3Normalize(XMLoadFloat3(&normal)));
}

float PlaneCollisionComponent::GetOffset() const
{
	XMFLOAT3 point = GetPosition();

	float d = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&mNormal), XMLoadFloat3(&point)));

	return d;
}