#ifndef PHYSICS_COMPONENT_H
#define PHYSICS_COMPONENT_H

#include "Component.h"
#include <DirectXMath.h>

using namespace DirectX;

class PhysicsComponent : public Component
{
public:
	float GetMass() const { return 1.0f / mInverseMass; }
	float GetInverseMass() const { return mInverseMass; }

	void SetMass(float mass) { mInverseMass = 1.0f / mass; }
	void SetInverseMass(float inverseMass) { mInverseMass = inverseMass; }

	XMFLOAT3X3 GetInertiaTensorWorld() const;
	XMFLOAT3X3 GetInverseInertiaTensorWorld() const;

	void SetInertiaTensor(const XMFLOAT3X3 &inertiaTensor);

	void AddForce(const XMFLOAT3 &force);
	void AddForceToBody(const XMFLOAT3 &force, const XMFLOAT3 &applicationPoint);

	void AddTorque(const XMFLOAT3 &torque);

	void ClearForceAccumulator() { mForceAccumulator = XMFLOAT3(); }
	void ClearTorqueAccumulator() { mTorqueAccumulator = XMFLOAT3(); }

	XMFLOAT3 GetForce() const { return mForceAccumulator; }
	XMFLOAT3 GetTorque() const { return mTorqueAccumulator; }

	float GetRoughness() const { return mRoughness; }
	float GetElasticity() const { return mElasticity; }
private:
	float mInverseMass = 0.0f;

	XMFLOAT3X3 mInertiaTensor;
	XMFLOAT3X3 mInverseInertiaTensor;

	XMFLOAT3 mForceAccumulator;
	XMFLOAT3 mTorqueAccumulator;

	float mRoughness;
	float mElasticity;
};

#endif  // PHYSICS_COMPONENT_H

