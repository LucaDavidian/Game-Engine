#include "ForceComponent.h"
#include "PhysicsComponent.h"
#include "Entity.h"

void ForceComponent::UpdateForce()
{
	Entity *owner = GetOwner();

	for (ForceGenerator *forceGenerator : mForceGenerators)
		forceGenerator->AddForce(owner);
}

void ForceComponent::ClearAccumulators()
{
	Entity *owner = GetOwner();

	owner->GetComponent<PhysicsComponent>()->ClearForceAccumulator();
	owner->GetComponent<PhysicsComponent>()->ClearTorqueAccumulator();
}