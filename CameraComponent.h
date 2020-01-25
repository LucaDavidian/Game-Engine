#ifndef CAMERA_COMPONENT_H
#define CAMERA_COMPONENT_H

#include "Component.h"
#include "EntitySystem.h"
#include <DirectXMath.h>

using namespace DirectX;

class PositionComponent;

class CameraComponent : public Component
{
public:
	struct Frustum
	{
		XMFLOAT3 nearTopLeft;
		XMFLOAT3 nearTopRight;
		XMFLOAT3 nearBottomLeft;
		XMFLOAT3 nearBottomRight;
		XMFLOAT3 farTopLeft;
		XMFLOAT3 farTopRight;
		XMFLOAT3 farBottomLeft;
		XMFLOAT3 farBottomRight;
	};
public:
	CameraComponent(float verticalFOV, float aspectRatio, float nearDistance, float farDistance, PositionComponent *positionComponent, const XMFLOAT3 &relativePosition, const XMFLOAT3 &relativeOrientationEulerAngles); 

	void Init() override { EntitySystem::GetInstance().SetCamera(GetOwner()); }

	void SetLens(float verticalFOV, float aspectRatio, float nearDistance, float farDistance);

	void SetActive(bool active) { mIsActive = active; }
	bool IsActive() const { return mIsActive; }

	Frustum const GetFrustum(float maxDistance) const;

	const XMFLOAT3 GetPosition() const;

	const XMFLOAT4X4 GetViewMatrix() const;
	const XMFLOAT4X4 GetInverseViewMatrix() const;

	const XMFLOAT4X4 &GetProjectionMatrix() const { return mProjectionMatrix; }

	float GetNearDistance() const { return mNearDistance; }
	float GetFarDistance() const { return mFarDistance; }
private:
	// associated entity's position component
	PositionComponent *mPositionComponent;

	bool mIsActive = false;

	// camera pose relative to associated entity (in entity's coordinate system)
	XMFLOAT3 mRelativePosition;                       
	XMFLOAT3 mRelativeOrientationEulerAngles;
	XMFLOAT4X4 mOffsetMatrix; 

	XMFLOAT4X4 mViewMatrix;

	// perspective projection parameters
	float mNearDistance;
	float mFarDistance;
	float mVerticalFOV;
	float mAspectRatio;
	XMFLOAT4X4 mProjectionMatrix;
};

#endif  // CAMERA_COMPONENT_H