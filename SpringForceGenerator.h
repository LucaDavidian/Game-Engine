#ifndef SPRING_FORCE_GENERATOR_H
#define SPRING_FORCE_GENERATOR_H

#include "ForceGenerator.h"
#include <DirectXMath.h>

using namespace DirectX;

class SpringForceGenerator : public ForceGenerator
{
public:
	SpringForceGenerator(const XMFLOAT3 *anchorPoint, const XMFLOAT3 &bodyPoint, float elasticConstant, float restLength, float damping) : mAnchorPoint(anchorPoint), mBodyPoint(bodyPoint), mElasticConstant(elasticConstant), mRestLength(restLength), mDamping(damping) {}

	void AddForce(Entity *entity) override;
private:
	const XMFLOAT3 *mAnchorPoint;
	const XMFLOAT3 mBodyPoint;
	float mElasticConstant;
	float mRestLength;
	float mDamping;
};

#endif  // SPRING_FORCE_GENERATOR_H

