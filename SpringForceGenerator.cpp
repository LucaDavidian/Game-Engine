#include "SpringForceGenerator.h"
#include "PositionComponent.h"
#include "MotionComponent.h"
#include "PhysicsComponent.h"
#include "Entity.h"

void SpringForceGenerator::AddForce(Entity *entity)
{
	PositionComponent *positionComponent = entity->GetComponent<PositionComponent>();
	MotionComponent *motionComponent = entity->GetComponent<MotionComponent>();
	PhysicsComponent *physicsComponent = entity->GetComponent<PhysicsComponent>();

	XMFLOAT4X4 worldMatrix = positionComponent->GetWorldMatrix();  
	XMVECTOR applicationPointWorld = XMVector3Transform(XMLoadFloat3(&mBodyPoint), XMLoadFloat4x4(&worldMatrix));

	XMVECTOR fromAnchorPoint = applicationPointWorld - XMLoadFloat3(mAnchorPoint);
	
	float length = XMVectorGetX(XMVector3Length(fromAnchorPoint));
	float dl = length - mRestLength;

	XMVECTOR direction = XMVector3Normalize(fromAnchorPoint);

	XMVECTOR springForce = -direction * mElasticConstant * dl;

	XMFLOAT3 velocity = motionComponent->GetVelocity();

	float speed = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&velocity), direction));
	
	XMVECTOR damperForce = -direction * speed * mDamping;

	XMFLOAT3 force;
	XMStoreFloat3(&force, springForce + damperForce);

	physicsComponent->AddForceToBody(force, mBodyPoint);
}