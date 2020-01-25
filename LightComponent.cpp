#include "LightComponent.h"

LightComponent::LightComponent(Type type, const XMFLOAT3 &color, float range, float intensity, float spotLightAngle, PositionComponent *positionComponent)
	: mType(type), mColor(color), mRange(range), mIntensity(intensity), mSpotlightAngle(spotLightAngle)
{
}