#include "LightSwitchInputComponent.h"
#include "InputSystem.h"
#include "PositionComponent.h"
#include "LightComponent.h"
#include "Entity.h"

LightSwitchInputComponent::LightSwitchInputComponent(PositionComponent *positionComponent) : InputComponent(positionComponent)
{
	
}

void LightSwitchInputComponent::Init()
{
	InputComponent::Init();

	InputSystem::GetInstance().RegisterKeyPressEvent(this, &LightSwitchInputComponent::OnLightSwitch);
}

void LightSwitchInputComponent::OnLightSwitch(int key)
{
	if (key == 'l')
	{
		LightComponent *light = GetOwner()->GetComponent<LightComponent>();
		light->SetEnabled(light->IsEnabled() ? false : true);
	}
}