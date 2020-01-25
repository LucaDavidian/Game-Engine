#ifndef MOTION_COMPONENT_H
#define MOTION_COMPONENT_H

#include "Component.h"

#include <DirectXMath.h>

using namespace DirectX;

class MotionComponent : public Component
{
public:
	MotionComponent(const XMFLOAT3 &initialVelocity = XMFLOAT3(), const XMFLOAT3 &initialAngularVelocity = XMFLOAT3()) : mVelocity(initialVelocity), mAngularVelocity(initialAngularVelocity) {}

	const XMFLOAT3 GetVelocity() const { return mVelocity; }
	const XMFLOAT3 GetAngularVelocity() const { return mAngularVelocity; }

	void SetVelocity(const XMFLOAT3 &velocity) { mVelocity = velocity; }
	void SetAngularVelocity(const XMFLOAT3 & angularVelocity) { mAngularVelocity = angularVelocity; }

	void AddVelocity(const XMFLOAT3 &velocity) { XMStoreFloat3(&mVelocity, XMLoadFloat3(&mVelocity) + XMLoadFloat3(&velocity)); }
	void AddAngularVelocity(const XMFLOAT3 &angularVelocity) { XMStoreFloat3(&mAngularVelocity, XMLoadFloat3(&mAngularVelocity) + XMLoadFloat3(&angularVelocity)); }

	void SetLastFrameDeltaVelocityLinear(const XMFLOAT3 &lastFrameDeltaVelocityLinear) { mLastFrameDeltaAccelerationLinear = lastFrameDeltaVelocityLinear; }
	void SetLastFrameDeltaVelocityAngular(const XMFLOAT3 &lastFrameDeltaAccelerationAngular) { mLastFrameDeltaAccelerationAngular = lastFrameDeltaAccelerationAngular; }

	XMFLOAT3 const GetLastFrameDeltaVelocityLinear() const { return mLastFrameDeltaAccelerationLinear; }
	XMFLOAT3 const GetLastFrameDeltaVelocityAngular() const { return mLastFrameDeltaAccelerationAngular; }
private:
	XMFLOAT3 mVelocity;
	XMFLOAT3 mAngularVelocity;

	XMFLOAT3 mLastFrameDeltaAccelerationLinear;
	XMFLOAT3 mLastFrameDeltaAccelerationAngular;
};

#endif  // MOTION_COMPONENT_H

