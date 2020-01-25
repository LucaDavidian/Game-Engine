#ifndef PICKER_H
#define PICKER_H

#ifdef FAST_DELEGATES
	#include "fast delegates/delegate.hpp"
#else
	#include "delegates/delegate.hpp"
#endif  // FAST_DELEGATES

#include "InputSystem.h"
#include "CameraComponent.h"
#include <DirectXMath.h>

using namespace DirectX;

class Picker
{
public:
	Picker(CameraComponent *camera) : mCamera(camera) { InputSystem::GetInstance().RegisterMousePressEvent(this, &Picker::CreateRay); }

	~Picker() { if (mConnection) mConnection->Disconnect(); delete mConnection; }

	void CreateRay(int x, int y);

	XMFLOAT3 const GetRay() const { return mRay; }
	XMFLOAT3 const GetOrigin() const { return mCamera->GetPosition(); }

	void Connect(ConnectionBase *connection) { mConnection = connection; }
private:
	XMFLOAT3 mRay;
	CameraComponent *mCamera;

	ConnectionBase *mConnection;
};

#endif  // PICKER_H