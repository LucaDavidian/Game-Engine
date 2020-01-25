#include "PositionComponent.h"

PositionComponent::PositionComponent(const XMFLOAT3 &position, const XMFLOAT3 &eulerAngles, const XMFLOAT3 &scale)
	: mPosition(position), mOrientationEulerAngles(eulerAngles), mScale(scale)
{
	// calculate rotation matrix
	// XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(mOrientationEulerAngles.x, mOrientationEulerAngles.y, mOrientationEulerAngles.z);

	// calculate elemental rotation matrices
	XMMATRIX roll = XMMatrixRotationZ(mOrientationEulerAngles.z);
	XMMATRIX pitch = XMMatrixRotationX(mOrientationEulerAngles.x);
	XMMATRIX yaw = XMMatrixRotationY(mOrientationEulerAngles.y);
	// elemental rotations are applied as roll + pitch + yaw (extrinsic)
	XMMATRIX rotationMatrix = XMMatrixMultiply(XMMatrixMultiply(roll, pitch), yaw); 

	// calculate translation matrix
	XMMATRIX translationMatrix = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);

	// calculate scale matrix
	XMMATRIX scaleMatrix = XMMatrixScaling(mScale.x, mScale.y, mScale.z);

	// calculate world matrix (rotation + translation)
	XMMATRIX worldMatrix = XMMatrixMultiply(rotationMatrix, translationMatrix);
	XMStoreFloat4x4(&mWorldMatrix, worldMatrix);

	// calculate scaled world matrix (scale + rotation + translation)
	XMMATRIX worldMatrixScale = XMMatrixMultiply(scaleMatrix, worldMatrix);	
	XMStoreFloat4x4(&mWorldMatrixScale, worldMatrixScale);

	// calculate inverse world matrix
	XMMATRIX inverseWorldMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&mWorldMatrix));
	XMStoreFloat4x4(&mInverseWorldMatrix, inverseWorldMatrix);

	// calculate orientation quaternion 
	// XMVECTOR orientationQuaternion = XMQuaternionRotationRollPitchYaw(mOrientationEulerAngles.x, mOrientationEulerAngles.y, mOrientationEulerAngles.z);
	// XMStoreFloat4(&mOrientationQuaternion, orientationQuaternion);

	// calculate orientation quaternion 
	XMVECTOR rollQuaternion = XMVectorSet(0.0f, 0.0f, sin(mOrientationEulerAngles.z / 2.0f), cos(mOrientationEulerAngles.z / 2.0f));
	XMVECTOR pitchQuaternion = XMVectorSet(sin(mOrientationEulerAngles.x / 2.0f), 0.0f, 0.0f, cos(mOrientationEulerAngles.x / 2.0f));
	XMVECTOR yawQuaternion = XMVectorSet(0.0f, sin(mOrientationEulerAngles.y / 2.0f), 0.0f, cos(mOrientationEulerAngles.y / 2.0f));

	XMStoreFloat4(&mOrientationQuaternion, XMQuaternionMultiply(rollQuaternion, XMQuaternionMultiply(pitchQuaternion, yawQuaternion)));
}

void PositionComponent::SetScale(const XMFLOAT3 &scale)
{
	mScale = scale;

	// remove scale from world matrix
	XMVECTOR v1 = XMVectorSet(mWorldMatrix._11, mWorldMatrix._12, mWorldMatrix._13, 1.0f);
	XMVECTOR v2 = XMVectorSet(mWorldMatrix._21, mWorldMatrix._22, mWorldMatrix._23, 1.0f);
	XMVECTOR v3 = XMVectorSet(mWorldMatrix._31, mWorldMatrix._32, mWorldMatrix._33, 1.0f);

	// normalize matrix rows
	float scaleX = XMVectorGetX(XMVector3Length(v1));
	float scaleY = XMVectorGetX(XMVector3Length(v2));
	float scaleZ = XMVectorGetX(XMVector3Length(v3));

	for (int i = 0; i < 3; i++)
		mWorldMatrix.m[0][i] /= scaleX;

	for (int i = 0; i < 3; i++)
		mWorldMatrix.m[1][i] /= scaleY;

	for (int i = 0; i < 3; i++)
		mWorldMatrix.m[2][i] /= scaleZ;

	// calculate new scale matrix
	XMMATRIX newScaleMatrix = XMMatrixScaling(mScale.x, mScale.y, mScale.z);

	// update world matrix
	XMMATRIX newWorldMatrixScale = XMMatrixMultiply(newScaleMatrix, XMLoadFloat4x4(&mWorldMatrix));
	XMStoreFloat4x4(&mWorldMatrixScale, newWorldMatrixScale);
}

void PositionComponent::SetPosition(const XMFLOAT3 &position)
{
	mPosition = position;

	// update scaled world matrix (update last row)
	mWorldMatrixScale.m[3][0] = mPosition.x;
	mWorldMatrixScale.m[3][1] = mPosition.y;
	mWorldMatrixScale.m[3][2] = mPosition.z;

	// update world matrix (update last row)
	mWorldMatrix.m[3][0] = mPosition.x;
	mWorldMatrix.m[3][1] = mPosition.y;
	mWorldMatrix.m[3][2] = mPosition.z;

	// update inverse world matrix
	// XMMATRIX inverseWorldMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&mWorldMatrix));
	// XMStoreFloat4x4(&mInverseWorldMatrix, inverseWorldMatrix);

	// update inverse world matrix 
	XMFLOAT3 x = GetAxisX();
	float tx = XMVectorGetX(XMVector3Dot(-XMLoadFloat3(&mPosition), XMLoadFloat3(&x)));
	XMFLOAT3 y = GetAxisY();
	float ty = XMVectorGetX(XMVector3Dot(-XMLoadFloat3(&mPosition), XMLoadFloat3(&y)));
	XMFLOAT3 z = GetAxisZ();
	float tz = XMVectorGetX(XMVector3Dot(-XMLoadFloat3(&mPosition), XMLoadFloat3(&z)));

	mInverseWorldMatrix.m[3][0] = tx;
	mInverseWorldMatrix.m[3][1] = ty;
	mInverseWorldMatrix.m[3][2] = tz;
}

void PositionComponent::SetOrientationEulerAngles(const XMFLOAT3 &orientationEulerAngles)
{
	// update euler angles
	mOrientationEulerAngles = orientationEulerAngles;

	// calculate new rotation matrix
	// XMMATRIX newRotationMatrix = XMMatrixRotationRollPitchYaw(mOrientationEulerAngles.x, mOrientationEulerAngles.y, mOrientationEulerAngles.z);

	// calculate new elemental rotation matrices
	XMMATRIX roll = XMMatrixRotationZ(mOrientationEulerAngles.z);
	XMMATRIX pitch = XMMatrixRotationX(mOrientationEulerAngles.x);
	XMMATRIX yaw = XMMatrixRotationY(mOrientationEulerAngles.y);
	// elemental rotations are applied as roll + pitch + yaw (extrinsic)
	XMMATRIX newRotationMatrix = XMMatrixMultiply(XMMatrixMultiply(roll, pitch), yaw);

	// calculate translation matrix
	XMMATRIX translationMatrix = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);

	// calculate scale matrix
	XMMATRIX scaleMatrix = XMMatrixScaling(mScale.x, mScale.y, mScale.z);

	// update world matrix (rotation + translation)
	XMMATRIX worldMatrix = XMMatrixMultiply(newRotationMatrix, translationMatrix);
	XMStoreFloat4x4(&mWorldMatrix, worldMatrix);

	// update scaled world matrix (scale + rotation + translation)
	XMMATRIX worldMatrixScale = XMMatrixMultiply(scaleMatrix, worldMatrix);
	XMStoreFloat4x4(&mWorldMatrixScale, worldMatrixScale);

	// update inverse world matrix
	XMMATRIX inverseWorldMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&mWorldMatrix));
	XMStoreFloat4x4(&mInverseWorldMatrix, inverseWorldMatrix);

	// update orientation quaternion 
	// XMVECTOR orientationQuaternion = XMQuaternionRotationRollPitchYaw(mOrientationEulerAngles.x, mOrientationEulerAngles.y, mOrientationEulerAngles.z);
	// XMStoreFloat4(&mOrientationQuaternion, orientationQuaternion);

	// update orientation quaternion 
	XMVECTOR rollQuaternion = XMVectorSet(0.0f, 0.0f, sin(mOrientationEulerAngles.z / 2.0f), cos(mOrientationEulerAngles.z / 2.0f));
	XMVECTOR pitchQuaternion = XMVectorSet(sin(mOrientationEulerAngles.x / 2.0f), 0.0f, 0.0f, cos(mOrientationEulerAngles.x / 2.0f));
	XMVECTOR yawQuaternion = XMVectorSet(0.0f, sin(mOrientationEulerAngles.y / 2.0f), 0.0f, cos(mOrientationEulerAngles.y / 2.0f));

	XMStoreFloat4(&mOrientationQuaternion, XMQuaternionMultiply(rollQuaternion, XMQuaternionMultiply(pitchQuaternion, yawQuaternion)));
}

void PositionComponent::SetOrientationQuaternion(const XMFLOAT4 &orientationQuaternion)
{
	// update orientation quaternion
	mOrientationQuaternion = orientationQuaternion;

	// calculate new scale, rotation and translation matrices
	XMMATRIX rotation = XMMatrixRotationQuaternion(XMLoadFloat4(&mOrientationQuaternion));
	XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition));
	XMMATRIX scale = XMMatrixScalingFromVector(XMLoadFloat3(&mScale));

	// update world matrix
	XMMATRIX worldMatrix = XMMatrixMultiply(rotation, translation);
	XMMATRIX worldMatrixScale = XMMatrixMultiply(scale, worldMatrix);

	XMStoreFloat4x4(&mWorldMatrix, worldMatrix);
	XMStoreFloat4x4(&mInverseWorldMatrix, XMMatrixInverse(nullptr, worldMatrix));
	XMStoreFloat4x4(&mWorldMatrixScale, worldMatrixScale);
}

const XMFLOAT3 PositionComponent::GetAxisX() const
{
	return XMFLOAT3(mWorldMatrix._11, mWorldMatrix._12, mWorldMatrix._13);
}

const XMFLOAT3 PositionComponent::GetAxisY() const
{
	return XMFLOAT3(mWorldMatrix._21, mWorldMatrix._22, mWorldMatrix._23);
}

const XMFLOAT3 PositionComponent::GetAxisZ() const
{
	return XMFLOAT3(mWorldMatrix._31, mWorldMatrix._32, mWorldMatrix._33);
}

void PositionComponent::Translate(const XMFLOAT3 &translationVector)
{
	XMFLOAT3 newPosition;
	XMStoreFloat3(&newPosition, XMLoadFloat3(&mPosition) + XMLoadFloat3(&translationVector));

	SetPosition(newPosition);
}

void PositionComponent::MoveForward(float distance)
{
	XMFLOAT3 newPosition;
	XMFLOAT3 axisZ = GetAxisZ();
	XMStoreFloat3(&newPosition, XMLoadFloat3(&mPosition) + XMLoadFloat3(&axisZ) * distance);

	SetPosition(newPosition);
}

void PositionComponent::MoveRight(float distance)
{
	XMFLOAT3 newPosition;
	XMFLOAT3 axisX = GetAxisX();
	XMStoreFloat3(&newPosition, XMLoadFloat3(&mPosition) + XMLoadFloat3(&axisX) * distance);

	SetPosition(newPosition);
}

void PositionComponent::MoveUp(float distance)
{
	XMFLOAT3 newPosition;
	XMFLOAT3 axisY = GetAxisY();
	XMStoreFloat3(&newPosition, XMLoadFloat3(&mPosition) + XMLoadFloat3(&axisY) * distance);

	SetPosition(newPosition);
}

void PositionComponent::Rotate(const XMFLOAT3 &deltaOrientationVector)
{
	XMVECTOR deltaOrientationQuaternionV = XMLoadFloat3(&deltaOrientationVector);
	XMVECTOR orientationQuaternionV = XMLoadFloat4(&mOrientationQuaternion);

	XMVECTOR newOrientationQuaternionV = XMQuaternionNormalize(orientationQuaternionV + XMQuaternionMultiply(orientationQuaternionV, deltaOrientationQuaternionV / 2.0f));

	XMFLOAT4 orientationQuaternion;
	XMStoreFloat4(&orientationQuaternion, newOrientationQuaternionV);

	SetOrientationQuaternion(orientationQuaternion);
}

void PositionComponent::RotateRoll(float angle)
{
	/**** set euler angles ****/

	 float roll = mOrientationEulerAngles.z + XMConvertToRadians(angle);
	 SetOrientationEulerAngles(XMFLOAT3(mOrientationEulerAngles.x, mOrientationEulerAngles.y, roll));

	/**** increment world matrix (out of sync euler angles) ****/

	//XMMATRIX rotationZ = XMMatrixRotationZ(XMConvertToRadians(angle));
	//XMMATRIX worldMatrix = XMLoadFloat4x4(&mWorldMatrix);

	//XMMATRIX newWorldMatrix = XMMatrixMultiply(rotationZ, worldMatrix);

	//XMStoreFloat4x4(&mWorldMatrix, newWorldMatrix);
	//XMStoreFloat4x4(&mInverseWorldMatrix, XMMatrixInverse(nullptr, newWorldMatrix));

	/**** increment rotation quaternion (out of sync euler angles)  ****/

	//XMVECTOR rotationQuaternionZ = XMQuaternionRotationRollPitchYaw(0.0f, 0.0f, XMConvertToRadians(angle));
	//XMVECTOR orientationQuaternion = XMLoadFloat4(&mOrientationQuaternion);
	//
	//XMStoreFloat4(&mOrientationQuaternion, XMQuaternionMultiply(rotationQuaternionZ, orientationQuaternion));

	//XMMATRIX scale = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
	//XMMATRIX translation = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
	//XMMATRIX rotation = XMMatrixRotationQuaternion(XMLoadFloat4(&mOrientationQuaternion));

	//XMMATRIX newWorldMatrix = XMMatrixMultiply(scale, XMMatrixMultiply(rotation, translation));
	//XMStoreFloat4x4(&mWorldMatrix, newWorldMatrix);
	//XMStoreFloat4x4(&mInverseWorldMatrix, XMMatrixInverse(nullptr, newWorldMatrix));
}

void PositionComponent::RotatePitch(float angle)
{
	/**** set euler angles ****/

	 float pitch = mOrientationEulerAngles.x + XMConvertToRadians(angle);
	 SetOrientationEulerAngles(XMFLOAT3(pitch, mOrientationEulerAngles.y, mOrientationEulerAngles.z));

	/**** increment world matrix (out of sync euler angles)  ****/

	//XMMATRIX rotationX = XMMatrixRotationX(XMConvertToRadians(angle));
	//XMMATRIX worldMatrix = XMLoadFloat4x4(&mWorldMatrix);

	//XMMATRIX newWorldMatrix = XMMatrixMultiply(rotationX, worldMatrix);

	//XMStoreFloat4x4(&mWorldMatrix, newWorldMatrix);
	//XMStoreFloat4x4(&mInverseWorldMatrix, XMMatrixInverse(nullptr, newWorldMatrix));

	/**** increment rotation quaternion (out of sync euler angles)  ****/

	//XMVECTOR rotationQuaternionX = XMQuaternionRotationRollPitchYaw(XMConvertToRadians(angle), 0.0f, 0.0f);
	//XMVECTOR orientationQuaternion = XMLoadFloat4(&mOrientationQuaternion);

	//XMStoreFloat4(&mOrientationQuaternion, XMQuaternionMultiply(rotationQuaternionX, orientationQuaternion));

	//XMMATRIX scale = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
	//XMMATRIX translation = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
	//XMMATRIX rotation = XMMatrixRotationQuaternion(XMLoadFloat4(&mOrientationQuaternion));

	//XMMATRIX newWorldMatrix = XMMatrixMultiply(scale, XMMatrixMultiply(rotation, translation));
	//XMStoreFloat4x4(&mWorldMatrix, newWorldMatrix);
	//XMStoreFloat4x4(&mInverseWorldMatrix, XMMatrixInverse(nullptr, newWorldMatrix));
}

void PositionComponent::RotateYaw(float angle)
{
	/**** set euler angles ****/

	 float yaw = mOrientationEulerAngles.y + XMConvertToRadians(angle);
	 SetOrientationEulerAngles(XMFLOAT3(mOrientationEulerAngles.x, yaw, mOrientationEulerAngles.z));

	/**** increment world matrix (out of sync euler angles)  ****/

	//XMMATRIX rotationY = XMMatrixRotationY(XMConvertToRadians(angle));
	//XMMATRIX worldMatrix = XMLoadFloat4x4(&mWorldMatrix);

	//XMMATRIX newWorldMatrix = XMMatrixMultiply(rotationY, worldMatrix);

	//XMStoreFloat4x4(&mWorldMatrix, newWorldMatrix);
	//XMStoreFloat4x4(&mInverseWorldMatrix, XMMatrixInverse(nullptr, newWorldMatrix));

	/**** increment rotation quaternion (out of sync euler angles)  ****/

	//XMVECTOR rotationQuaternionY = XMQuaternionRotationRollPitchYaw(0.0f, XMConvertToRadians(angle), 0.0f);
	//XMVECTOR orientationQuaternion = XMLoadFloat4(&mOrientationQuaternion);

	//XMStoreFloat4(&mOrientationQuaternion, XMQuaternionMultiply(rotationQuaternionY, orientationQuaternion));

	//XMMATRIX scale = XMMatrixScaling(mScale.x, mScale.y, mScale.z);
	//XMMATRIX translation = XMMatrixTranslation(mPosition.x, mPosition.y, mPosition.z);
	//XMMATRIX rotation = XMMatrixRotationQuaternion(XMLoadFloat4(&mOrientationQuaternion));

	//XMMATRIX newWorldMatrix = XMMatrixMultiply(scale, XMMatrixMultiply(rotation, translation));
	//XMStoreFloat4x4(&mWorldMatrix, newWorldMatrix);
	//XMStoreFloat4x4(&mInverseWorldMatrix, XMMatrixInverse(nullptr, newWorldMatrix));
}