#ifndef ARROW_INPUT_COMPONENT_H
#define ARROW_INPUT_COMPONENT_H

#include "InputComponent.h"

class ArrowInputComponent : public InputComponent
{
public:
	ArrowInputComponent(PositionComponent *positionComponent) : InputComponent(positionComponent) {}

	void OnKeyDown(int key) override;
};

#endif  // ARROW_INPUT_COMPONENT_H

