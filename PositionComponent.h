#ifndef POSITION_COMPONENT_H
#define POSITION_COMPONENT_H

#include "Component.h"
#include <DirectXMath.h>

using namespace DirectX;

/**** DirectX Math convention for Tait-Bryan/cardan/nautical angles: roll-pitch-yaw = z-x-y extrinsic / yaw-pitch-roll = y-x'-z" intrinsic ****/
/**** euler angles parameter is x = pitch, y = yaw, z = roll ****/

class PositionComponent : public Component
{
public:
	PositionComponent(const XMFLOAT3 &position, const XMFLOAT3 &eulerAngles, const XMFLOAT3 &scale);

	XMFLOAT3 GetScale() const { return mScale; }
	void SetScale(const XMFLOAT3 &scale);

	const XMFLOAT3 &GetPosition() const { return mPosition; }
	void SetPosition(const XMFLOAT3 &position);

	XMFLOAT3 const GetOrientationEulerAngles() const { return mOrientationEulerAngles; }
	void SetOrientationEulerAngles(const XMFLOAT3 &orientationEulerAngles);

	XMFLOAT4 const GetOrientationQuaternion() const { return mOrientationQuaternion; }
	void SetOrientationQuaternion(const XMFLOAT4 &orientationQuaternion);

	const XMFLOAT3 GetAxisX() const;
	const XMFLOAT3 GetAxisY() const;
	const XMFLOAT3 GetAxisZ() const;

	void Translate(const XMFLOAT3 &translationVector);

	void MoveForward(float distance);
	void MoveRight(float distance);
	void MoveUp(float distance);

	void Rotate(const XMFLOAT3 &orientationVector);

	void RotateRoll(float angle);
	void RotatePitch(float angle);
	void RotateYaw(float angle);

	const XMFLOAT4X4 &GetWorldMatrixScale() const { return mWorldMatrixScale; }
	const XMFLOAT4X4 &GetWorldMatrix() const { return mWorldMatrix; }    
	const XMFLOAT4X4 &GetInverseWorldMatrix() const { return mInverseWorldMatrix; } 
private:
	// entity's pose (position + orientation) relative to world coordinate system
	XMFLOAT3 mPosition;     
	XMFLOAT3 mOrientationEulerAngles;    // orientation is a rotation from a fixed reference frame (world)
	XMFLOAT4 mOrientationQuaternion;
	XMFLOAT3 mScale;

	XMFLOAT4X4 mWorldMatrixScale;
	XMFLOAT4X4 mWorldMatrix;
	XMFLOAT4X4 mInverseWorldMatrix;
};

#endif  // POSITION_COMPONENT_H