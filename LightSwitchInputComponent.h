#ifndef LIGHTSWITCH_INPUT_COMPONENT_H
#define LIGHTSWITCH_INPUT_COMPONENT_H

#include "InputComponent.h"

class PositionComponent;

class LightSwitchInputComponent : public InputComponent
{
public:
	LightSwitchInputComponent(PositionComponent *positionComponent);

	void Init() override;

	void OnLightSwitch(int key);
};

#endif  // LIGHTSWITCH_INPUT_COMPONENT_H

