#include "GravityForceGenerator.h"
#include "Entity.h"
#include "PhysicsComponent.h"

void GravityForceGenerator::AddForce(Entity *entity)
{
	PhysicsComponent *physicsComponent = entity->GetComponent<PhysicsComponent>();

	XMFLOAT3 gravity;
	XMStoreFloat3(&gravity, XMLoadFloat3(&mGravity) * physicsComponent->GetMass());

	physicsComponent->AddForce(gravity);
}