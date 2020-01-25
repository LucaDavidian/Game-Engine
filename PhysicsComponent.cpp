#include "PhysicsComponent.h"
#include "PositionComponent.h"
#include "Entity.h"

void PhysicsComponent::AddForce(const XMFLOAT3 &force)
{ 
	// add force to force accumulator
	XMStoreFloat3(&mForceAccumulator, XMLoadFloat3(&mForceAccumulator) + XMLoadFloat3(&force)); 
}

void PhysicsComponent::AddTorque(const XMFLOAT3 &torque)
{
	// add torque to torque accumulator
	XMStoreFloat3(&mTorqueAccumulator, XMLoadFloat3(&mTorqueAccumulator) + XMLoadFloat3(&torque));
}

void PhysicsComponent::AddForceToBody(const XMFLOAT3 &force, const XMFLOAT3 &applicationPoint)
{
	// calculate torque
	Entity *owner = GetOwner();
	
	PositionComponent *positionComponent = owner->GetComponent<PositionComponent>();

	XMFLOAT3 position = positionComponent->GetPosition();
	XMFLOAT4X4 worldMatrix = positionComponent->GetWorldMatrix();  

	XMVECTOR applicationPointWorld = XMVector3Transform(XMLoadFloat3(&applicationPoint), XMLoadFloat4x4(&worldMatrix));

	XMVECTOR distance = applicationPointWorld - XMLoadFloat3(&position);

	XMFLOAT3 torque;
	XMStoreFloat3(&torque, XMVector3Cross(distance, XMLoadFloat3(&force)));

	// add force and torque to accumulators
	AddForce(force);
	AddTorque(torque);
}

void PhysicsComponent::SetInertiaTensor(const XMFLOAT3X3 &inertiaTensor)
{
	mInertiaTensor = inertiaTensor;

	XMStoreFloat3x3(&mInverseInertiaTensor, XMMatrixInverse(nullptr, XMLoadFloat3x3(&inertiaTensor)));
}

XMFLOAT3X3 PhysicsComponent::GetInertiaTensorWorld() const
{
	Entity *owner = GetOwner();
	PositionComponent *positionComponent = owner->GetComponent<PositionComponent>();

	XMFLOAT4X4 worldMatrix = positionComponent->GetWorldMatrix();
	XMFLOAT4X4 inverseWorldMatrix = positionComponent->GetInverseWorldMatrix();

	XMFLOAT3X3 inertiaTensorWorld;
	XMStoreFloat3x3(&inertiaTensorWorld, XMMatrixMultiply(XMLoadFloat4x4(&inverseWorldMatrix), XMMatrixMultiply(XMLoadFloat3x3(&mInertiaTensor), XMLoadFloat4x4(&worldMatrix))));

	return inertiaTensorWorld;
}

XMFLOAT3X3 PhysicsComponent::GetInverseInertiaTensorWorld() const
{
	XMFLOAT3X3 inertiaTensorWorld = GetInertiaTensorWorld();

	XMFLOAT3X3 inverseInertiaTensorWorld;
	XMStoreFloat3x3(&inverseInertiaTensorWorld, XMMatrixInverse(nullptr, XMLoadFloat3x3(&inertiaTensorWorld)));

	return inverseInertiaTensorWorld;
}
