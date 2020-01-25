#include "PhysicsSystem.h"
#include "EntitySystem.h"
#include "PositionComponent.h"
#include "MotionComponent.h"
#include "PhysicsComponent.h"
#include "ForceComponent.h"

void PhysicsSystem::Update(float dt)
{
	for (Entity *entity : EntitySystem::GetInstance().GetEntities())
	{
		if (entity->HasComponent<MotionComponent>() && entity->HasComponent<PhysicsComponent>() && entity->HasComponent<ForceComponent>())
		{
			// get physics related components
			PositionComponent *positionComponent = entity->GetComponent<PositionComponent>();
			MotionComponent *motionComponent = entity->GetComponent<MotionComponent>();
			ForceComponent *forceComponent = entity->GetComponent<ForceComponent>();
			PhysicsComponent *physicsComponent = entity->GetComponent<PhysicsComponent>();

			// accumulate forces
			forceComponent->UpdateForce();

			/**** integrate linear equation of motion ****/
			
			// update position
			XMFLOAT3 velocity = motionComponent->GetVelocity();
			
			XMFLOAT3 deltaPosition;
			XMStoreFloat3(&deltaPosition, XMLoadFloat3(&velocity) * dt);
			
			positionComponent->Translate(deltaPosition);

			// calculate linear acceleration
			XMFLOAT3 force = physicsComponent->GetForce();		
			XMVECTOR linearAccelerationV = XMLoadFloat3(&force) * physicsComponent->GetInverseMass();

			// update velocity
			XMFLOAT3 deltaVelocity;
			XMStoreFloat3(&deltaVelocity, linearAccelerationV * dt);
			
			motionComponent->AddVelocity(deltaVelocity);
			motionComponent->SetLastFrameDeltaVelocityLinear(deltaVelocity);

			/**** integrate angular equation of motion ****/

			// update orientation
			XMFLOAT3 angularVelocity = motionComponent->GetAngularVelocity();
			XMFLOAT4 orientationQuaternion = positionComponent->GetOrientationQuaternion();

			XMFLOAT4 newOrientationQuaternion;
			XMStoreFloat4(&newOrientationQuaternion, XMQuaternionNormalize(XMLoadFloat4(&orientationQuaternion) + XMQuaternionMultiply(XMLoadFloat4(&orientationQuaternion), XMLoadFloat3(&angularVelocity) * dt / 2.0f)));

			positionComponent->SetOrientationQuaternion(newOrientationQuaternion);

			// calculate angular acceleration: DW = I^(-1) * (M - w X Iw) 
			XMFLOAT3X3 inertiaTensor = physicsComponent->GetInertiaTensorWorld();
			XMFLOAT3X3 inverseInertiaTensorWorld = physicsComponent->GetInverseInertiaTensorWorld();

			XMVECTOR Iw = XMVector3TransformNormal(XMLoadFloat3(&angularVelocity), XMLoadFloat3x3(&inertiaTensor));   // Iw
			XMVECTOR transport = XMVector3Cross(XMLoadFloat3(&angularVelocity), Iw);   // w X Ix

			XMFLOAT3 torque = physicsComponent->GetTorque();
			XMVECTOR angularAccelerationV = XMVector3TransformNormal(XMLoadFloat3(&torque) - transport, XMLoadFloat3x3(&inverseInertiaTensorWorld));

			// update angular velocity
			XMFLOAT3 deltaAngularVelocity;
			XMStoreFloat3(&deltaAngularVelocity, angularAccelerationV * dt);

			motionComponent->AddAngularVelocity(deltaAngularVelocity);
			motionComponent->SetLastFrameDeltaVelocityAngular(deltaAngularVelocity);

			// clear force accumulator
			forceComponent->ClearAccumulators(); 
		}
	}
}