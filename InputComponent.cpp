#include "InputComponent.h"
#include "InputSystem.h"
#include "PositionComponent.h"
#include "LightComponent.h"

InputComponent::InputComponent(PositionComponent *positionComponent) : mPositionComponent(positionComponent)
{
	
}

void InputComponent::Init()
{
	InputSystem::GetInstance().RegisterMouseMoveEvent(this, &InputComponent::OnMouseMove);
	InputSystem::GetInstance().RegisterKeyDownEvent(this, &InputComponent::OnKeyDown);
}

void InputComponent::OnKeyDown(int key)
{
	switch (key)
	{
		case 'W':
			mPositionComponent->MoveForward(0.1f);
			break;
		case 'A':
			mPositionComponent->MoveRight(-0.1f);
			break;
		case 'S':
			mPositionComponent->MoveForward(-0.1f);
			break;
		case 'D':
			mPositionComponent->MoveRight(0.1f);
			break;
		case 'Q':
			mPositionComponent->MoveUp(0.1f);
			break;
		case 'E':
			mPositionComponent->MoveUp(-0.1f);
			break;
		case 'Z':
			//mPositionComponent->RotateRoll(0.2f);
			break;
		case 'X':
			//mPositionComponent->RotateRoll(-0.2f);
			break;
	}
}

void InputComponent::OnMouseMove(int x, int y)
{
	static int oldX = x, oldY = y;

	int xOffset = x - oldX;
	int yOffset = y - oldY;

	float deltaYaw = xOffset * 0.3f;
	float deltaPitch = yOffset * 0.3f;

	mPositionComponent->RotateYaw(deltaYaw);
	mPositionComponent->RotatePitch(deltaPitch);

	oldX = x;
	oldY = y;
}