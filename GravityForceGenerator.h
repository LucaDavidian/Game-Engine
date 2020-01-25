#ifndef GRAVITY_FORCE_GENERATOR_H
#define GRAVITY_FORCE_GENERATOR_H

#include "ForceGenerator.h"
#include <DirectXMath.h>

using namespace DirectX;

class GravityForceGenerator : public ForceGenerator
{
public:
	GravityForceGenerator(float gravityPull = -9.81f) : mGravity(XMFLOAT3(0.0f, -gravityPull, 0.0f)) {}

	void AddForce(Entity *entity) override;
private:
	XMFLOAT3 mGravity;
};

#endif  // GRAVITY_FORCE_GENERATOR_H



