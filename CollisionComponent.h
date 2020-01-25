#ifndef COLLISION_COMPONENT_H
#define COLLISION_COMPONENT_H

#include "Component.h"
#include <DirectXMath.h>

using namespace DirectX;

class CollisionComponent : public Component
{
public:
	enum class Type { BOX, SPHERE, PLANE, };
public:
	CollisionComponent(Type type, const XMFLOAT3 &relativePosition);

	Type GetType() const { return mType; }

	XMFLOAT3 const GetPosition() const;

	const XMFLOAT3 GetAxis(int axis) const;

	XMFLOAT4X4 const GetWorldMatrix() const;
	XMFLOAT4X4 const GetInverseWorldMatrix() const;

	void SetMovable(bool movable) { mIsMovable = movable; }
	bool IsMovable() const { return mIsMovable; }
private:
	Type mType;
	
	XMFLOAT3 mRelativePosition;
	XMFLOAT4X4 mOffsetMatrix;

	bool mIsMovable = false;
};

#endif  // COLLISION_COMPONENT_H

