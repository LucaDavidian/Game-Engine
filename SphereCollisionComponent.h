#ifndef SPHERE_COLLISION_COMPONENT_H
#define SPHERE_COLLISION_COMPONENT_H

#include "CollisionComponent.h"

class SphereCollisionComponent : public CollisionComponent
{
public:
	SphereCollisionComponent(float radius, const XMFLOAT3 &relativePosition = XMFLOAT3()) : CollisionComponent(Type::SPHERE, relativePosition), mRadius(radius) {}

	float GetRadius() const { return mRadius; }
private:
	float mRadius;
};

#endif  // SPHERE_COLLISION_COMPONENT_H
