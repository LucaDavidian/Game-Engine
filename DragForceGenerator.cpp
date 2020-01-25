#include "DragForceGenerator.h"
#include "MotionComponent.h"
#include "PhysicsComponent.h"
#include "Entity.h"

void DragForceGenerator::AddForce(Entity *entity)
{
	MotionComponent *motionComponent = entity->GetComponent<MotionComponent>();
	PhysicsComponent *physicsComponent = entity->GetComponent<PhysicsComponent>();

	// add linear drag
	XMFLOAT3 velocity = motionComponent->GetVelocity();

	float speed = XMVectorGetX(XMVector3Length(XMLoadFloat3(&velocity)));
	XMVECTOR direction = XMVector3Normalize(XMLoadFloat3(&velocity));

	XMFLOAT3 force;
	XMVECTOR linear = -direction * speed * mTranslationDrag / 10.0f;
	XMVECTOR quadratic = -direction * speed * speed * mTranslationDrag;
	XMStoreFloat3(&force, linear + quadratic);

	physicsComponent->AddForce(force);

	// add rotational drag
	XMFLOAT3 angularVelocity = motionComponent->GetAngularVelocity();

	float spin = XMVectorGetX(XMVector3Length(XMLoadFloat3(&angularVelocity)));
	XMVECTOR rotationAxis = XMVector3Normalize(XMLoadFloat3(&angularVelocity));

	XMFLOAT3 torque;
	XMStoreFloat3(&torque, -rotationAxis * spin * spin * mRotationDrag);

	physicsComponent->AddTorque(torque);
}