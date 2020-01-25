#include "Contact.h"
#include "Entity.h"
#include "PositionComponent.h"
#include "MotionComponent.h"
#include "PhysicsComponent.h"

Contact::Contact(const XMFLOAT3 &contactPoint, const XMFLOAT3 &contactNormal, float penetration, Entity *entityA, Entity *entityB) 
	: mContactPoint(contactPoint), mContactNormal(contactNormal), mPenetration(penetration), mEntities{ entityA, entityB }
{
	if (!mEntities[0])
	{
		mEntities[0] = mEntities[1];
		mEntities[1] = nullptr;
		XMStoreFloat3(&mContactNormal, -XMLoadFloat3(&mContactNormal));
	}

	//PhysicsComponent *physicsComponent1 = mEntities[0]->GetComponent<PhysicsComponent>();
	//PhysicsComponent *physicsComponent2 = mEntities[1]->GetComponent<PhysicsComponent>();

	//// calculate coefficient of restitution
	//float elasticity1 = physicsComponent1->GetElasticity();
	//float elasticity2 = physicsComponent2->GetElasticity();
	mCoefficientOfRestitution = 0.6f; // mEntities[0]->GetComponent<PhysicsComponent>()->GetElasticity() + mEntities[0]->GetComponent<PhysicsComponent>()->GetElasticity() / 2.0f;

	// calculate friction
	/*float roughness1 = physicsComponent1->GetRoughness();
	float roughness2 = physicsComponent2->GetRoughness();*/

	mFriction = 0.7f;  // TODO: calculate friction from roughness
}

void Contact::CalculateContactData()
{
	// calculate contact point local base
	XMVECTOR yV = XMVector3Normalize(XMLoadFloat3(&mContactNormal));

	XMVECTOR xV;
	if (mContactNormal.y != 1.0f)
		xV = XMVector3Normalize(XMVector3Cross(yV, XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)));
	else
		xV = XMVector3Normalize(XMVector3Cross(yV, XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)));

	XMVECTOR zV = XMVector3Normalize(XMVector3Cross(xV, yV));

	XMFLOAT3 x;
	XMStoreFloat3(&x, xV);

	XMFLOAT3 y;
	XMStoreFloat3(&y, yV);

	XMFLOAT3 z;
	XMStoreFloat3(&z, zV);

	// set transformation matrices
	mContactToWorldMatrix._11 = x.x;
	mContactToWorldMatrix._12 = x.y;
	mContactToWorldMatrix._13 = x.z;
	mContactToWorldMatrix._21 = y.x;
	mContactToWorldMatrix._22 = y.y;
	mContactToWorldMatrix._23 = y.z;
	mContactToWorldMatrix._31 = z.x;
	mContactToWorldMatrix._32 = z.y;
	mContactToWorldMatrix._33 = z.z;

	mWorldToContactMatrix._11 = x.x;
	mWorldToContactMatrix._21 = x.y;
	mWorldToContactMatrix._31 = x.z;
	mWorldToContactMatrix._12 = y.x;
	mWorldToContactMatrix._22 = y.y;
	mWorldToContactMatrix._32 = y.z;
	mWorldToContactMatrix._13 = z.x;
	mWorldToContactMatrix._23 = z.y;
	mWorldToContactMatrix._33 = z.z;

	// get contact point offset vector from center of mass
	PositionComponent *positionComponent1 = mEntities[0]->GetComponent<PositionComponent>();
	XMFLOAT3 mCenterOfMass1 = positionComponent1->GetPosition();

	XMVECTOR offset1V = XMLoadFloat3(&mContactPoint) - XMLoadFloat3(&mCenterOfMass1);
	XMStoreFloat3(&mContactPointOffset[0], offset1V);

	// set skew-symmetric (anti-symmetric) matrix for contact point offset vector
	mContactPointOffsetSkewMatrix[0]._11 = 0.0f;
	mContactPointOffsetSkewMatrix[0]._12 = mContactPointOffset[0].z;
	mContactPointOffsetSkewMatrix[0]._13 = -mContactPointOffset[0].y;
	mContactPointOffsetSkewMatrix[0]._21 = -mContactPointOffset[0].z;
	mContactPointOffsetSkewMatrix[0]._22 = 0.0f;
	mContactPointOffsetSkewMatrix[0]._23 = mContactPointOffset[0].x;
	mContactPointOffsetSkewMatrix[0]._31 = mContactPointOffset[0].y;
	mContactPointOffsetSkewMatrix[0]._32 = -mContactPointOffset[0].x;
	mContactPointOffsetSkewMatrix[0]._33 = 0.0f;

	if (mEntities[1])
	{
		PositionComponent *positionComponent2 = mEntities[1]->GetComponent<PositionComponent>();
		XMFLOAT3 mCenterOfMass2 = positionComponent2->GetPosition();

		XMVECTOR offset2V = XMLoadFloat3(&mContactPoint) - XMLoadFloat3(&mCenterOfMass2);
		XMStoreFloat3(&mContactPointOffset[1], offset2V);

		mContactPointOffsetSkewMatrix[1]._11 = 0.0f;
		mContactPointOffsetSkewMatrix[1]._12 = mContactPointOffset[1].z;
		mContactPointOffsetSkewMatrix[1]._13 = -mContactPointOffset[1].y;
		mContactPointOffsetSkewMatrix[1]._21 = -mContactPointOffset[1].z;
		mContactPointOffsetSkewMatrix[1]._22 = 0.0f;
		mContactPointOffsetSkewMatrix[1]._23 = mContactPointOffset[1].x;
		mContactPointOffsetSkewMatrix[1]._31 = mContactPointOffset[1].y;
		mContactPointOffsetSkewMatrix[1]._32 = -mContactPointOffset[1].x;
		mContactPointOffsetSkewMatrix[1]._33 = 0.0f;
	}

	// calculate contact point relative velocity in world coordinates V_a - V_b
	MotionComponent *motionComponent1 = mEntities[0]->GetComponent<MotionComponent>();
	XMFLOAT3 linearVelocity1 = motionComponent1->GetVelocity();
	XMFLOAT3 angularVelocity1 = motionComponent1->GetAngularVelocity();
	
	XMVECTOR contactPointRelativeVelocityWorldV = XMLoadFloat3(&linearVelocity1) + XMVector3Cross(XMLoadFloat3(&angularVelocity1), XMLoadFloat3(&mContactPointOffset[0]));

	if (mEntities[1])
	{
		MotionComponent *motionComponent2 = mEntities[1]->GetComponent<MotionComponent>();

		XMFLOAT3 linearVelocity2 = motionComponent2->GetVelocity();
		XMFLOAT3 angularVelocity2 = motionComponent2->GetAngularVelocity();
		
		contactPointRelativeVelocityWorldV -= XMLoadFloat3(&linearVelocity2) + XMVector3Cross(XMLoadFloat3(&angularVelocity2), XMLoadFloat3(&mContactPointOffset[1]));
	}

	// calculate relative velocity in contact local coordinates
	XMVECTOR contactPointRelativeVelocityLocalV = XMVector3TransformNormal(contactPointRelativeVelocityWorldV, XMLoadFloat3x3(&mWorldToContactMatrix));
	XMStoreFloat3(&mContactPointRelativeVelocityLocal, contactPointRelativeVelocityLocalV);

	// calculate closing velocity V_c = -V_ab * n_ba (local y component)
	float closingVelocity = -mContactPointRelativeVelocityLocal.y;

	// closing velocity by acceleration due to forces acting during this frame (resting contacts) V_acc
	XMFLOAT3 deltaLinearVelocity = motionComponent1->GetLastFrameDeltaVelocityLinear();
	XMFLOAT3 deltaAngularVelocity = motionComponent1->GetLastFrameDeltaVelocityAngular();

	XMVECTOR relativeVelocityFromLastFrameAcceleration = XMLoadFloat3(&deltaLinearVelocity) + XMVector3Cross(XMLoadFloat3(&deltaAngularVelocity), XMLoadFloat3(&mContactPointOffset[0]));

	if (mEntities[1])
	{
		MotionComponent *motionComponent2 = mEntities[1]->GetComponent<MotionComponent>();

		XMFLOAT3 deltaLinearVelocity = motionComponent2->GetLastFrameDeltaVelocityLinear();
		XMFLOAT3 deltaAngularVelocity = motionComponent2->GetLastFrameDeltaVelocityAngular();

		relativeVelocityFromLastFrameAcceleration -= XMLoadFloat3(&deltaLinearVelocity) + XMVector3Cross(XMLoadFloat3(&deltaAngularVelocity), XMLoadFloat3(&mContactPointOffset[1]));
	}

	float closingVelocityFromLastFrameAcceleration = -XMVectorGetX(XMVector3Dot(relativeVelocityFromLastFrameAcceleration, XMLoadFloat3(&mContactNormal)));

	// TODO: reduce coefficient of restitution for small velocities

	// calculate desired delta closing velocity  DV_c = V'_c - V_c = -V_c * c - V_c 
	//mDeltaClosingVelocity = -closingVelocity * (1 + mCoefficientOfRestitution);

	// calculate desired delta closing velocity (micocollisions) DV_c = V'_c - V_c = -(V_c - V_acc) * c - V_c 
	mDeltaClosingVelocity = -(closingVelocity - closingVelocityFromLastFrameAcceleration) * mCoefficientOfRestitution - closingVelocity;
}

XMFLOAT3 Contact::CalculateFrictionlessImpulse()
{
	// calculate contact point delta velocity - linear component
	XMVECTOR deltaLinearVelocityPerUnitImpulse = XMLoadFloat3(&mContactNormal) * mEntities[0]->GetComponent<PhysicsComponent>()->GetInverseMass();
	XMVECTOR deltaContactPointVelocityLinearComponent = deltaLinearVelocityPerUnitImpulse;

	// calculate unit torque
	XMVECTOR unitImpulsiveTorque = XMVector3Cross(XMLoadFloat3(&mContactPointOffset[0]), XMLoadFloat3(&mContactNormal));

	// calculate contact point delta velocity - angular component
	XMVECTOR deltaAngularVelocityPerUnitImpulse = XMVector3TransformNormal(unitImpulsiveTorque, XMLoadFloat3x3(&mEntities[0]->GetComponent<PhysicsComponent>()->GetInverseInertiaTensorWorld()));
	XMVECTOR deltaContactPointVelocityAngularComponent = XMVector3Cross(deltaAngularVelocityPerUnitImpulse, XMLoadFloat3(&mContactPointOffset[0]));

	// calculate contact point total delta velocity
	XMVECTOR deltaContactPointVelocityPerUnitImpulse = deltaContactPointVelocityLinearComponent + deltaContactPointVelocityAngularComponent;

	// same calculations for body 1 (contact normal has opposite direction)
	if (mEntities[1])
	{		
		// calculate contact point delta velocity - linear component
		XMVECTOR deltaLinearVelocityPerUnitImpulse = -XMLoadFloat3(&mContactNormal) * mEntities[1]->GetComponent<PhysicsComponent>()->GetInverseMass();   // impulse has opposite direction
		XMVECTOR deltaContactPointVelocityLinearComponent = deltaLinearVelocityPerUnitImpulse;

		// calculate unit torque
		XMVECTOR unitImpulsiveTorque = XMVector3Cross(XMLoadFloat3(&mContactPointOffset[1]), -XMLoadFloat3(&mContactNormal));  // impulse has opposite direction

		// calculate contact point delta velocity - angular component
		XMVECTOR deltaAngularVelocityPerUnitImpulse = XMVector3TransformNormal(unitImpulsiveTorque, XMLoadFloat3x3(&mEntities[1]->GetComponent<PhysicsComponent>()->GetInverseInertiaTensorWorld()));
		XMVECTOR deltaContactPointVelocityAngularComponent = XMVector3Cross(deltaAngularVelocityPerUnitImpulse, XMLoadFloat3(&mContactPointOffset[1]));		

		// calculate contact point total delta velocity and add to contact point delta relative velocity
		deltaContactPointVelocityPerUnitImpulse -= deltaContactPointVelocityLinearComponent + deltaContactPointVelocityAngularComponent;
	}

	// calculate delta relative velocity in contact coordinates
	XMVECTOR deltaContactPointVelocityPerUnitImpulseLocal = XMVector3TransformNormal(deltaContactPointVelocityPerUnitImpulse, XMLoadFloat3x3(&mWorldToContactMatrix));

	// calculate the normal impulse (normal component is local y component)
	float impulseIntensity = mDeltaClosingVelocity / -XMVectorGetY(deltaContactPointVelocityPerUnitImpulseLocal);
	XMFLOAT3 impulseLocal(0.0f, impulseIntensity, 0.0f);

	// calculate impulse in world coordinate and return impulse
	XMFLOAT3 impulse;
	XMStoreFloat3(&impulse, XMVector3TransformNormal(XMLoadFloat3(&impulseLocal), XMLoadFloat3x3(&mContactToWorldMatrix)));

	return impulse;
}

XMFLOAT3 Contact::CalculateFrictionImpulse()
{
	XMMATRIX impulseToDeltaAngularVelocity = XMMatrixMultiply(XMLoadFloat3x3(&mContactPointOffsetSkewMatrix[0]), XMMatrixMultiply(XMLoadFloat3x3(&mEntities[0]->GetComponent<PhysicsComponent>()->GetInverseInertiaTensorWorld()), -XMLoadFloat3x3(&mContactPointOffsetSkewMatrix[0])));
	
	float inverseMass = mEntities[0]->GetComponent<PhysicsComponent>()->GetInverseMass();

	if (mEntities[1])
	{
		impulseToDeltaAngularVelocity += XMMatrixMultiply(XMLoadFloat3x3(&mContactPointOffsetSkewMatrix[1]), XMMatrixMultiply(XMLoadFloat3x3(&mEntities[1]->GetComponent<PhysicsComponent>()->GetInverseInertiaTensorWorld()), -XMLoadFloat3x3(&mContactPointOffsetSkewMatrix[1])));
		
		inverseMass += mEntities[1]->GetComponent<PhysicsComponent>()->GetInverseMass();
	}

	XMMATRIX impulseToDeltaLinearVelocity = XMMatrixIdentity() * inverseMass;

	XMMATRIX impulseToDeltaAngularVelocityLocal = XMMatrixMultiply(XMLoadFloat3x3(&mContactToWorldMatrix), XMMatrixMultiply(impulseToDeltaAngularVelocity, XMLoadFloat3x3(&mWorldToContactMatrix)));

	XMMATRIX impulseToDeltaVelocityLocalM = impulseToDeltaLinearVelocity + impulseToDeltaAngularVelocityLocal;

	XMFLOAT3X3 impulseToDeltaVelocitylocal;
	XMStoreFloat3x3(&impulseToDeltaVelocitylocal, impulseToDeltaVelocityLocalM);

	XMMATRIX deltaVelocityToImpulseLocal = XMMatrixInverse(nullptr, impulseToDeltaVelocityLocalM);

	// delta velocities
	XMVECTOR deltaVelocityLocal = XMVectorSet(-mContactPointRelativeVelocityLocal.x, -mDeltaClosingVelocity, -mContactPointRelativeVelocityLocal.z, 0.0f);

	XMVECTOR impulseLocalV = XMVector3TransformNormal(deltaVelocityLocal, deltaVelocityToImpulseLocal);

	XMFLOAT3 impulseLocal;
	XMStoreFloat3(&impulseLocal, impulseLocalV);

	float planarImpulse = sqrt(impulseLocal.x * impulseLocal.x + impulseLocal.z * impulseLocal.z);

	if (planarImpulse > impulseLocal.y * mFriction)
	{
		impulseLocal.x /= planarImpulse;
		impulseLocal.z /= planarImpulse;

		impulseLocal.y = impulseToDeltaVelocitylocal._12 * mFriction * impulseLocal.x + impulseToDeltaVelocitylocal._22 + impulseToDeltaVelocitylocal._32 * mFriction * impulseLocal.z;
		impulseLocal.y = -mDeltaClosingVelocity / impulseLocal.y;

		impulseLocal.x *= mFriction * impulseLocal.y;
		impulseLocal.z *= mFriction * impulseLocal.y;

		impulseLocalV = XMLoadFloat3(&impulseLocal);
	}

	XMFLOAT3 impulseWorld;
	XMStoreFloat3(&impulseWorld, XMVector3TransformNormal(impulseLocalV, XMLoadFloat3x3(&mContactToWorldMatrix)));

	return impulseWorld;
}

void Contact::ResolveVelocity(XMFLOAT3(&deltaLinearVelocity)[2], XMFLOAT3(&deltaAngularVelocity)[2])
{
	XMFLOAT3 impulse;

	if (mFriction) // calculate collision impulse (with friction)
		impulse = CalculateFrictionImpulse();
	else          // calculate collision impulse (no friction)
		impulse = CalculateFrictionlessImpulse();

	// calculate delta linear velocity
	XMVECTOR deltaLinearVelocityV = XMLoadFloat3(&impulse) * mEntities[0]->GetComponent<PhysicsComponent>()->GetInverseMass();

	// calculate impulsive torque and delta angular velocity
	XMVECTOR impulsiveTorque = XMVector3Cross(XMLoadFloat3(&mContactPointOffset[0]), XMLoadFloat3(&impulse));
	XMVECTOR deltaAngularVelocityV = XMVector3TransformNormal(impulsiveTorque, XMLoadFloat3x3(&mEntities[0]->GetComponent<PhysicsComponent>()->GetInverseInertiaTensorWorld()));

	// update body's velocities
	XMStoreFloat3(&deltaLinearVelocity[0], deltaLinearVelocityV);
	XMStoreFloat3(&deltaAngularVelocity[0], deltaAngularVelocityV);

	mEntities[0]->GetComponent<MotionComponent>()->AddVelocity(deltaLinearVelocity[0]);
	mEntities[0]->GetComponent<MotionComponent>()->AddAngularVelocity(deltaAngularVelocity[0]);

	// same calculations for body 1 (contact impulse has opposite direction)
	if (mEntities[1])
	{
		// calculate delta linear velocity
		XMVECTOR deltaLinearVelocityV = -XMLoadFloat3(&impulse) * mEntities[1]->GetComponent<PhysicsComponent>()->GetInverseMass();  // impulse has opposite direction

		// calculate impulsive torque and delta angular velocity
		XMVECTOR impulsiveTorque = XMVector3Cross(XMLoadFloat3(&mContactPointOffset[1]), -XMLoadFloat3(&impulse));  // impulse has opposite direction
		XMVECTOR deltaAngularVelocityV = XMVector3TransformNormal(impulsiveTorque, XMLoadFloat3x3(&mEntities[1]->GetComponent<PhysicsComponent>()->GetInverseInertiaTensorWorld()));
		
		// update body's velocities
		XMStoreFloat3(&deltaLinearVelocity[1], deltaLinearVelocityV);
		XMStoreFloat3(&deltaAngularVelocity[1], deltaAngularVelocityV);

		mEntities[1]->GetComponent<MotionComponent>()->AddVelocity(deltaLinearVelocity[1]);
		mEntities[1]->GetComponent<MotionComponent>()->AddAngularVelocity(deltaAngularVelocity[1]);
	}
}

void Contact::ResolveInterpenetration(XMFLOAT3(&deltaPosition)[2], XMFLOAT3(&deltaOrientation)[2])
{
	float linearInertia[2]{};
	float angularInertia[2]{};
	float totalInertia = 0.0f;

	// calculate contact point delta velocity - linear component
	XMVECTOR deltaLinearVelocityPerUnitImpulse = XMLoadFloat3(&mContactNormal) * mEntities[0]->GetComponent<PhysicsComponent>()->GetInverseMass();
	XMVECTOR deltaContactPointVelocityLinearComponent = deltaLinearVelocityPerUnitImpulse;

	// calculate unit torque
	XMVECTOR unitImpulsiveTorque = XMVector3Cross(XMLoadFloat3(&mContactPointOffset[0]), XMLoadFloat3(&mContactNormal));

	// calculate contact point delta velocity - angular component
	XMVECTOR deltaAngularVelocityPerUnitImpulse = XMVector3TransformNormal(unitImpulsiveTorque, XMLoadFloat3x3(&mEntities[0]->GetComponent<PhysicsComponent>()->GetInverseInertiaTensorWorld()));
	XMVECTOR deltaContactPointVelocityAngularComponent = XMVector3Cross(deltaAngularVelocityPerUnitImpulse, XMLoadFloat3(&mContactPointOffset[0]));

	// calculate linar and angular inertia for body 1
	linearInertia[0] = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&mContactNormal), deltaContactPointVelocityLinearComponent));
	angularInertia[0] = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&mContactNormal), deltaContactPointVelocityAngularComponent));

	// calculate total inertia (linear + angular)
	totalInertia += linearInertia[0] + angularInertia[0];

	// same calculations for body 1 (contact normal has opposite direction)
	if (mEntities[1])
	{
		// calculate contact point delta velocity - linear component
		XMVECTOR deltaLinearVelocityPerUnitImpulse = -XMLoadFloat3(&mContactNormal) * mEntities[1]->GetComponent<PhysicsComponent>()->GetInverseMass();
		XMVECTOR deltaContactPointVelocityLinearComponent = deltaLinearVelocityPerUnitImpulse;

		// calculate unit torque
		XMVECTOR unitImpulsiveTorque = XMVector3Cross(XMLoadFloat3(&mContactPointOffset[1]), -XMLoadFloat3(&mContactNormal));

		// calculate contact point delta velocity - angular component
		XMVECTOR deltaAngularVelocityPerUnitImpulse = XMVector3TransformNormal(unitImpulsiveTorque, XMLoadFloat3x3(&mEntities[1]->GetComponent<PhysicsComponent>()->GetInverseInertiaTensorWorld()));
		XMVECTOR deltaContactPointVelocityAngularComponent = XMVector3Cross(deltaAngularVelocityPerUnitImpulse, XMLoadFloat3(&mContactPointOffset[1]));

		// calculate linar and angular inertia for body 2
		linearInertia[1] = XMVectorGetX(XMVector3Dot(-XMLoadFloat3(&mContactNormal), deltaContactPointVelocityLinearComponent));    // inertia of body 2 is delta velocity in direction of -contactNormal
		angularInertia[1] = XMVectorGetX(XMVector3Dot(-XMLoadFloat3(&mContactNormal), deltaContactPointVelocityAngularComponent));  
		
		// add body 2 inertia to total inertia
		totalInertia += linearInertia[1] + angularInertia[1];
	}

	float linearMove[2]{};
	float angularMove[2]{};

	linearMove[0] = mPenetration * (linearInertia[0] / totalInertia);
	XMStoreFloat3(&deltaPosition[0], XMLoadFloat3(&mContactNormal) * linearMove[0]);

	angularMove[0] = mPenetration * (angularInertia[0] / totalInertia);

	// TODO: avoid big rotations (small rotation assumption)

	if (angularMove[0] == 0.0f)
		deltaOrientation[0] = XMFLOAT3(0.0f, 0.0f, 0.0f);
	else
	{
		XMVECTOR rotationDirection = XMVector3Cross(XMLoadFloat3(&mContactPointOffset[0]), XMLoadFloat3(&mContactNormal));
		XMVECTOR angularVelocity = XMVector3TransformNormal(rotationDirection, XMLoadFloat3x3(&mEntities[0]->GetComponent<PhysicsComponent>()->GetInverseInertiaTensorWorld()));
		XMVECTOR deltaOrientationVector = angularVelocity * angularMove[0];

		XMStoreFloat3(&deltaOrientation[0], deltaOrientationVector);
	}

	mEntities[0]->GetComponent<PositionComponent>()->Translate(deltaPosition[0]);
	mEntities[0]->GetComponent<PositionComponent>()->Rotate(deltaOrientation[0]);

	if (mEntities[1])
	{
		linearMove[1] = mPenetration * (linearInertia[1] / totalInertia);
		XMStoreFloat3(&deltaPosition[1], -XMLoadFloat3(&mContactNormal) * linearMove[1]);

		angularMove[1] = mPenetration * (angularInertia[1] / totalInertia);

		// TODO: avoid big rotations (small rotation assumption)

		if (angularMove[1] == 0.0f)
			deltaOrientation[1] = XMFLOAT3(0.0f, 0.0f, 0.0f);
		else
		{
			XMVECTOR rotationDirection = XMVector3Normalize(XMVector3Cross(XMLoadFloat3(&mContactPointOffset[1]), -XMLoadFloat3(&mContactNormal)));
			XMVECTOR angularVelocity = XMVector3TransformNormal(rotationDirection, XMLoadFloat3x3(&mEntities[1]->GetComponent<PhysicsComponent>()->GetInverseInertiaTensorWorld()));
			XMVECTOR deltaOrientationVector = angularVelocity * angularMove[1];

			XMStoreFloat3(&deltaOrientation[1], deltaOrientationVector);
		}

		mEntities[1]->GetComponent<PositionComponent>()->Translate(deltaPosition[1]);
		mEntities[1]->GetComponent<PositionComponent>()->Rotate(deltaOrientation[1]);
	}
}
