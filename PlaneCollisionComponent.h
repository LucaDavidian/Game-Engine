#ifndef PLANE_COLLISION_COMPONENT_H
#define PLANE_COLLISION_COMPONENT_H

#include "CollisionComponent.h"

class PlaneCollisionComponent : public CollisionComponent
{
public:
	PlaneCollisionComponent(const XMFLOAT3 &normal, const XMFLOAT3 &relativePosition = XMFLOAT3());

	XMFLOAT3 const GetNormal() const { return mNormal; }
	float GetOffset() const;
private:
	XMFLOAT3 mNormal;
};

#endif  // PLANE_COLLISION_COMPONENT_H