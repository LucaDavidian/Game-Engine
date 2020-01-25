#include "ArrowInputComponent.h"
#include "PositionComponent.h"
#include "InputSystem.h"

void ArrowInputComponent::OnKeyDown(int key)
{
	switch (key)
	{
	case VK_UP:
		mPositionComponent->MoveForward(0.1f);
		break;
	case VK_LEFT:
		mPositionComponent->MoveRight(-0.1f);
		break;
		case VK_DOWN:
		mPositionComponent->MoveForward(-0.1f);
		break;
	case VK_RIGHT:
		mPositionComponent->MoveRight(0.1f);
		break;
	case VK_PRIOR:
		mPositionComponent->MoveUp(0.1f);
		break;
	case VK_NEXT:
		mPositionComponent->MoveUp(-0.1f);
		break;
	}
}