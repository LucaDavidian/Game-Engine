#ifndef LIGHT_COMPONENT_H
#define LIGHT_COMPONENT_H

#include "Component.h"
#include <DirectXMath.h>

using namespace DirectX;

class PositionComponent;

class LightComponent : public Component
{
public:
	enum class Type { DIRECTIONAL, POINT, SPOT, };
public:
	LightComponent(Type type, const XMFLOAT3 &color, float range, float intensity, float spotLightAngle, PositionComponent *positionComponent);

	void SetEnabled(bool enabled) { mIsEnabled = enabled; }
	bool IsEnabled() const { return mIsEnabled; }

	Type GetType() const { return mType; }
	XMFLOAT3 GetColor() const { return mColor; }
	float GetRange() const { return mRange; }
	float GetIntensity() const { return mIntensity; }
	float GetSpotLightAngle() const { return mSpotlightAngle; }
private:
	PositionComponent *mPositionComponent;

	bool mIsEnabled = true;

	Type mType;
	XMFLOAT3 mColor;
	float mRange;
	float mIntensity;
	float mSpotlightAngle;
};

#endif  // LIGHT_COMPONENT_H
