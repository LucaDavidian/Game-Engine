#ifndef CONTACT_H
#define CONTACT_H

#include <DirectXMath.h>

using namespace DirectX;

class Entity;

class Contact
{
friend class CollisionSystem;
public:
	Contact(const XMFLOAT3 &contactPoint, const XMFLOAT3 &contactNormal, float penetration, Entity *entityA, Entity *entityB);

	void CalculateContactData();
	void ResolveVelocity(XMFLOAT3(&deltaLinearVelocity)[2], XMFLOAT3(&deltaAngularVelocity)[2]);    // apply impulse
	void ResolveInterpenetration(XMFLOAT3(&deltaPosition)[2], XMFLOAT3(&deltaOrientation)[2]);      // apply displacement
private:
	XMFLOAT3 CalculateFrictionlessImpulse();
	XMFLOAT3 CalculateFrictionImpulse();

	XMFLOAT3 mContactPoint;
	XMFLOAT3 mContactNormal;
	float mPenetration;  
	Entity *mEntities[2];

	XMFLOAT3X3 mContactToWorldMatrix;
	XMFLOAT3X3 mWorldToContactMatrix;

	XMFLOAT3 mContactPointOffset[2];
	XMFLOAT3X3 mContactPointOffsetSkewMatrix[2];

	XMFLOAT3 mContactPointRelativeVelocityLocal;

	float mDeltaClosingVelocity;

	float mCoefficientOfRestitution;
	float mFriction;
};

#endif  // CONTACT_H
