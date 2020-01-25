#pragma once

#include "CollisionComponent.h"

class BoxCollisionComponent : public CollisionComponent
{
public:
	BoxCollisionComponent(const XMFLOAT3 &halfSize, const XMFLOAT3 &relativePosition = XMFLOAT3()) : CollisionComponent(Type::BOX, relativePosition), mHalfSize(halfSize) {}

	XMFLOAT3 const GetHalfSize() const { return mHalfSize; }
private:
	XMFLOAT3 mHalfSize;
};

