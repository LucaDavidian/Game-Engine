#include "CollisionSystem.h"
#include "EntitySystem.h"
#include "SphereCollisionComponent.h"
#include "BoxCollisionComponent.h"
#include "PlaneCollisionComponent.h"

#include "Picker.h"

void CollisionSystem::DoCollisions()
{
	// check for collisions
	const Vector<Entity*> &entities = EntitySystem::GetInstance().GetEntities();

	for (int i = 0; i < entities.Size(); i++)
	{
		// picking
		if (mPicker && entities[i]->HasComponent<CollisionComponent>())
		{
			CollisionComponent *collisionComponent = entities[i]->GetComponent<CollisionComponent>();

			if (collisionComponent->GetType() == CollisionComponent::Type::SPHERE)
				RayAndSphereCollision(mPicker->GetRay(), mPicker->GetOrigin(), static_cast<SphereCollisionComponent*>(collisionComponent));
		}

		for (int j = i + 1; j < entities.Size(); j++)
		{
			if (!entities[i]->HasComponent<CollisionComponent>() || !entities[j]->HasComponent<CollisionComponent>())
				continue;

			CollisionComponent *collisionComponent1 = entities[i]->GetComponent<CollisionComponent>();
			CollisionComponent *collisionComponent2 = entities[j]->GetComponent<CollisionComponent>();

			// get collision geometry and calculate contacts
			if (collisionComponent1->GetType() == CollisionComponent::Type::BOX && collisionComponent2->GetType() == CollisionComponent::Type::BOX)
				BoxAndBoxCollision(static_cast<BoxCollisionComponent*>(collisionComponent1), static_cast<BoxCollisionComponent*>(collisionComponent2));
			else if (collisionComponent1->GetType() == CollisionComponent::Type::SPHERE && collisionComponent2->GetType() == CollisionComponent::Type::SPHERE)
				SphereAndSphereCollision(static_cast<SphereCollisionComponent*>(collisionComponent1), static_cast<SphereCollisionComponent*>(collisionComponent2));
			else if (collisionComponent1->GetType() == CollisionComponent::Type::PLANE)
			{
				if (collisionComponent2->GetType() == CollisionComponent::Type::BOX)
					BoxAndHalfSpaceCollision(static_cast<BoxCollisionComponent*>(collisionComponent2), static_cast<PlaneCollisionComponent*>(collisionComponent1));
				else if (collisionComponent2->GetType() == CollisionComponent::Type::SPHERE)
					SphereAndHalfSpaceCollision(static_cast<SphereCollisionComponent*>(collisionComponent2), static_cast<PlaneCollisionComponent*>(collisionComponent1));
			}
			else if (collisionComponent2->GetType() == CollisionComponent::Type::PLANE)
			{
				if (collisionComponent1->GetType() == CollisionComponent::Type::BOX)
					BoxAndHalfSpaceCollision(static_cast<BoxCollisionComponent*>(collisionComponent1), static_cast<PlaneCollisionComponent*>(collisionComponent2));
				else if (collisionComponent1->GetType() == CollisionComponent::Type::SPHERE)
					SphereAndHalfSpaceCollision(static_cast<SphereCollisionComponent*>(collisionComponent1), static_cast<PlaneCollisionComponent*>(collisionComponent2));
			}
			else if (collisionComponent1->GetType() == CollisionComponent::Type::BOX && collisionComponent2->GetType() == CollisionComponent::Type::SPHERE)
				BoxAndSphereCollision(static_cast<BoxCollisionComponent*>(collisionComponent1), static_cast<SphereCollisionComponent*>(collisionComponent2));
			else if (collisionComponent1->GetType() == CollisionComponent::Type::SPHERE && collisionComponent2->GetType() == CollisionComponent::Type::BOX)
				BoxAndSphereCollision(static_cast<BoxCollisionComponent*>(collisionComponent2), static_cast<SphereCollisionComponent*>(collisionComponent1));
		}
	}

	mPicker = nullptr;

	// resolve collisions
	if (mContacts.Size())
		ResolveContacts();
}

/**** contact resolver routine ****/
void CollisionSystem::ResolveContacts()
{
	static int numIterationPosition = 5;
	static int numIterationVelocity = 5; 

	for (Contact *contact : mContacts)
		contact->CalculateContactData();

	// resolve interpenetrations 
	while (numIterationPosition--)
	{
		// find max penetration in contact set
		float currentPenetration = 0.01f;  // threshold for interpenetration
		int index = -1;

		for (int i = 0; i < mContacts.Size(); i++)
		{
			if (mContacts[i]->mPenetration > currentPenetration)
			{
				currentPenetration = mContacts[i]->mPenetration;
				index = i;
			}
		}

		// if no penetration is found exit
		if (index == -1)
			break;

		// store linear and angular change for colliding entities
		XMFLOAT3 deltaPosition[2];
		XMFLOAT3 deltaOrientation[2];

		// resolve interpenetration - apply displacement
		//mContacts[index]->MatchAwakeState();
		mContacts[index]->ResolveInterpenetration(deltaPosition, deltaOrientation);

		// update other penetrations in contact set (entities in contact with resolved entities)
		for (int i = 0; i < mContacts.Size(); i++)
			for (int j = 0; j < 2; j++)
				if (mContacts[i]->mEntities[j])
					for (int k = 0; k < 2; k++)
						if (mContacts[i]->mEntities[j] == mContacts[index]->mEntities[k])
						{
							XMVECTOR deltaContactPointPositionLinear = XMLoadFloat3(&deltaPosition[k]);
							XMVECTOR deltaContactPointPositionAngular = XMVector3Cross(XMLoadFloat3(&deltaOrientation[k]), XMLoadFloat3(&mContacts[i]->mContactPointOffset[j]));
							XMVECTOR deltaContactPointPosition = deltaContactPointPositionLinear + deltaContactPointPositionAngular;

							float delta = XMVectorGetX(XMVector3Dot(deltaContactPointPosition, XMLoadFloat3(&mContacts[i]->mContactNormal)));

							if (j == 0)
								mContacts[i]->mPenetration -= delta;
							else  // j == 1
								mContacts[i]->mPenetration += delta;
						}
	}

	// resolve velocities 
	while (numIterationVelocity--)
	{
		float currentDeltaClosingVelocity = -0.01f;   // threshold for delta closing velocity
		int index = -1;

		for (int i = 0; i < mContacts.Size(); i++)
		{
			if (mContacts[i]->mDeltaClosingVelocity < currentDeltaClosingVelocity)
			{
				currentDeltaClosingVelocity = mContacts[i]->mDeltaClosingVelocity;
				index = i;
			}
		}

		if (index == -1)
			break;

		// store linear and angular velocity change for colliding entities
		XMFLOAT3 deltaLinearVelocity[2];
		XMFLOAT3 deltaAngularVelocity[2];

		// resolve velocity - apply impulse
		//mContacts[index]->MatchAwakeState();
		mContacts[index]->ResolveVelocity(deltaLinearVelocity, deltaAngularVelocity);

		// update other closing velocities in contact set (entities in contact with resolved entities)
		for (int i = 0; i < mContacts.Size(); i++)
			for (int j = 0; j < 2; j++)
				if (mContacts[i]->mEntities[j])
					for (int k = 0; k < 2; k++)
						if (mContacts[i]->mEntities[j] == mContacts[index]->mEntities[k])
						{
							XMVECTOR deltaContactPointVelocity = XMLoadFloat3(&deltaLinearVelocity[k]) + XMVector3Cross(XMLoadFloat3(&deltaAngularVelocity[k]), XMLoadFloat3(&mContacts[i]->mContactPointOffset[j]));

							if (j == 0)
								XMStoreFloat3(&mContacts[i]->mContactPointRelativeVelocityLocal, XMLoadFloat3(&mContacts[i]->mContactPointRelativeVelocityLocal) + deltaContactPointVelocity);
							else  // j == 1
								XMStoreFloat3(&mContacts[i]->mContactPointRelativeVelocityLocal, XMLoadFloat3(&mContacts[i]->mContactPointRelativeVelocityLocal) - deltaContactPointVelocity);

							// update contact point delta relative velocity
							mContacts[i]->mDeltaClosingVelocity = -XMVectorGetX(XMVector3Dot(-XMLoadFloat3(&mContacts[i]->mContactNormal), XMLoadFloat3(&mContacts[i]->mContactPointRelativeVelocityLocal))) * (1 + mContacts[i]->mCoefficientOfRestitution);
						}
	}

	numIterationPosition = 5;
	numIterationVelocity = 5;

	//// delete Contact objects and clear list of contacts
	//for (Contact *contact : mContacts)
	//{
	//	if (contact->mCollidables[0] && contact->mCollidables[0]->collisionComponent->isDestroyable)
	//		eventBus.Trigger(new DestroyEvent(contact->mCollidables[0]->collisionComponent->GetEntity()));

	//	if (contact->mCollidables[1] && contact->mCollidables[1]->collisionComponent->isDestroyable)
	//		eventBus.Trigger(new DestroyEvent(contact->mCollidables[1]->collisionComponent->GetEntity()));

	//	delete contact;
	//}

	// clear contacts buffer
	mContacts.Clear();
}

/**** separating axis theorem ****/
Vector<XMFLOAT3> CollisionSystem::GetSATAxes(BoxCollisionComponent *box1, BoxCollisionComponent *box2)
{
	Vector<XMFLOAT3> axes;

	XMFLOAT3 axisX1 = box1->GetAxis(0);
	XMFLOAT3 axisY1 = box1->GetAxis(1);
	XMFLOAT3 axisZ1 = box1->GetAxis(2);

	XMFLOAT3 axisX2 = box2->GetAxis(0);
	XMFLOAT3 axisY2 = box2->GetAxis(1);
	XMFLOAT3 axisZ2 = box2->GetAxis(2);

	axes.InsertLast(axisX1);
	axes.InsertLast(axisY1);
	axes.InsertLast(axisZ1);

	axes.InsertLast(axisX2);
	axes.InsertLast(axisY2);
	axes.InsertLast(axisZ2);

	XMFLOAT3 axis;

	XMStoreFloat3(&axis, XMVector3Cross(XMLoadFloat3(&axisX1), XMLoadFloat3(&axisX2)));
	axes.InsertLast(axis);
	XMStoreFloat3(&axis, XMVector3Cross(XMLoadFloat3(&axisX1), XMLoadFloat3(&axisY2)));
	axes.InsertLast(axis);
	XMStoreFloat3(&axis, XMVector3Cross(XMLoadFloat3(&axisX1), XMLoadFloat3(&axisZ2)));
	axes.InsertLast(axis);
	XMStoreFloat3(&axis, XMVector3Cross(XMLoadFloat3(&axisY1), XMLoadFloat3(&axisX2)));
	axes.InsertLast(axis);
	XMStoreFloat3(&axis, XMVector3Cross(XMLoadFloat3(&axisY1), XMLoadFloat3(&axisY2)));
	axes.InsertLast(axis);
	XMStoreFloat3(&axis, XMVector3Cross(XMLoadFloat3(&axisY1), XMLoadFloat3(&axisZ2)));
	axes.InsertLast(axis);
	XMStoreFloat3(&axis, XMVector3Cross(XMLoadFloat3(&axisZ1), XMLoadFloat3(&axisX2)));
	axes.InsertLast(axis);
	XMStoreFloat3(&axis, XMVector3Cross(XMLoadFloat3(&axisZ1), XMLoadFloat3(&axisY2)));
	axes.InsertLast(axis);
	XMStoreFloat3(&axis, XMVector3Cross(XMLoadFloat3(&axisZ1), XMLoadFloat3(&axisZ2)));
	axes.InsertLast(axis);

	return axes;
}

float CollisionSystem::PerformSAT(const XMFLOAT3 &axis, BoxCollisionComponent *box1, BoxCollisionComponent *box2)
{
	XMFLOAT3 boxPosition1 = box1->GetPosition();
	XMFLOAT3 boxPosition2 = box2->GetPosition();

	XMVECTOR centercenterOffsetV = XMLoadFloat3(&boxPosition1) - XMLoadFloat3(&boxPosition2);

	XMFLOAT3 centerOffset;
	XMStoreFloat3(&centerOffset, centercenterOffsetV);

	XMVECTOR axisV = XMVector3Normalize(XMLoadFloat3(&axis));

	float distance = fabs(XMVectorGetX(XMVector3Dot(axisV, centercenterOffsetV)));

	XMFLOAT3 boxHalfSize1 = box1->GetHalfSize();
	XMFLOAT3 boxHalfSize2 = box2->GetHalfSize();

	XMFLOAT3 axisX1 = box1->GetAxis(0);
	XMFLOAT3 axisY1 = box1->GetAxis(1);
	XMFLOAT3 axisZ1 = box1->GetAxis(2);

	XMFLOAT3 axisX2 = box2->GetAxis(0);
	XMFLOAT3 axisY2 = box2->GetAxis(1);
	XMFLOAT3 axisZ2 = box2->GetAxis(2);

	float proj1 = boxHalfSize1.x * fabs(XMVectorGetX(XMVector3Dot(axisV, XMLoadFloat3(&axisX1)))) +
		boxHalfSize1.y * fabs(XMVectorGetX(XMVector3Dot(axisV, XMLoadFloat3(&axisY1)))) +
		boxHalfSize1.z * fabs(XMVectorGetX(XMVector3Dot(axisV, XMLoadFloat3(&axisZ1))));

	float proj2 = boxHalfSize2.x * fabs(XMVectorGetX(XMVector3Dot(axisV, XMLoadFloat3(&axisX2)))) +
		boxHalfSize2.y * fabs(XMVectorGetX(XMVector3Dot(axisV, XMLoadFloat3(&axisY2)))) +
		boxHalfSize2.z * fabs(XMVectorGetX(XMVector3Dot(axisV, XMLoadFloat3(&axisZ2))));

	float overlap = (proj1 + proj2) - distance;

	return overlap;
}

/**** collision detection / contact data generation algorithms ****/

#include <limits>
void CollisionSystem::BoxAndBoxCollision(BoxCollisionComponent *box1, BoxCollisionComponent *box2)
{
	XMFLOAT3 contactPoint;
	XMFLOAT3 contactNormal;
	float minOverlap = 10000.0f; // std::numeric_limits<float>::max();

	// get axes for SAT test
	Vector<XMFLOAT3> axes = GetSATAxes(box1, box2);

	int axisIndex = 0;
	int index = 0;

	for (XMFLOAT3 axis : axes)
	{
		if (XMVectorGetX(XMVector3Length(XMLoadFloat3(&axis))) < 0.01f)
		{
			index++;
			continue;
		}

		float overlap = PerformSAT(axis, box1, box2);

		if (overlap < 0)  // found separating axis - early out
			return;

		if (overlap < minOverlap)
		{
			minOverlap = overlap;
			axisIndex = index;
		}

		index++;
	}

	float penetration = minOverlap;

	XMFLOAT3 boxPosition1 = box1->GetPosition();
	XMFLOAT3 boxPosition2 = box2->GetPosition();

	XMVECTOR centerOffsetV = XMLoadFloat3(&boxPosition1) - XMLoadFloat3(&boxPosition2);

	// generate contact data for each case
	if (axisIndex < 3)  // box1 face - box2 vertex
	{
		// calculate separating plane's normal
		XMVECTOR normalV = XMVector3Normalize(XMLoadFloat3(&axes[axisIndex]));

		// plane's normal is directed as box2 --> box1
		if (XMVectorGetX(XMVector3Dot(normalV, centerOffsetV)) < 0)
			normalV = -normalV;

		// find box2 support point (contact point)
		XMFLOAT3 axisX2 = box2->GetAxis(0);
		XMFLOAT3 axisY2 = box2->GetAxis(1);
		XMFLOAT3 axisZ2 = box2->GetAxis(2);
		
		XMFLOAT3 contactPointBox2 = box2->GetHalfSize();

		if (XMVectorGetX(XMVector3Dot(normalV, XMLoadFloat3(&axisX2))) < 0)
			contactPointBox2.x = -contactPointBox2.x;
		if (XMVectorGetX(XMVector3Dot(normalV, XMLoadFloat3(&axisY2))) < 0)
			contactPointBox2.y = -contactPointBox2.y;
		if (XMVectorGetX(XMVector3Dot(normalV, XMLoadFloat3(&axisZ2))) < 0)
			contactPointBox2.z = -contactPointBox2.z;

		XMFLOAT4X4 boxWorldMatrix2 = box2->GetWorldMatrix();
		XMStoreFloat3(&contactPoint, XMVector3Transform(XMLoadFloat3(&contactPointBox2), XMLoadFloat4x4(&boxWorldMatrix2)));

		XMStoreFloat3(&contactNormal, normalV);
	}
	else if (axisIndex < 6)   // box2 face - box1 vertex
	{
		// calculate separating plane's normal
		XMVECTOR normalV = XMVector3Normalize(XMLoadFloat3(&axes[axisIndex]));

		// plane's normal is directed as box2 --> box1
		if (XMVectorGetX(XMVector3Dot(normalV, centerOffsetV)) < 0)
			normalV = -normalV;

		// find box1 support point (contact point)
		XMFLOAT3 axisX1 = box1->GetAxis(0);
		XMFLOAT3 axisY1 = box1->GetAxis(1);
		XMFLOAT3 axisZ1 = box1->GetAxis(2);

		XMFLOAT3 collisionPointBox1 = box1->GetHalfSize();

		if (XMVectorGetX(XMVector3Dot(normalV, XMLoadFloat3(&axisX1))) > 0)
			collisionPointBox1.x = -collisionPointBox1.x;
		if (XMVectorGetX(XMVector3Dot(normalV, XMLoadFloat3(&axisY1))) > 0)
			collisionPointBox1.y = -collisionPointBox1.y;
		if (XMVectorGetX(XMVector3Dot(normalV, XMLoadFloat3(&axisZ1))) > 0)
			collisionPointBox1.z = -collisionPointBox1.z;

		XMFLOAT4X4 boxWorldMatrix1 = box1->GetWorldMatrix();
		XMStoreFloat3(&contactPoint, XMVector3Transform(XMLoadFloat3(&collisionPointBox1), XMLoadFloat4x4(&boxWorldMatrix1)));

		XMStoreFloat3(&contactNormal, normalV);
	}
	else  // box1 edge - box2 edge
	{
		axisIndex -= 6;
		int index1 = axisIndex / 3;
		int index2 = axisIndex % 3;

		XMFLOAT3 axisA = box1->GetAxis(index1);
		XMFLOAT3 axisB = box2->GetAxis(index2);

		XMVECTOR axisAV = XMLoadFloat3(&axisA);
		XMVECTOR axisBV = XMLoadFloat3(&axisB);

		// calculate separating plane's normal
		XMVECTOR normalV = XMVector3Normalize(XMVector3Cross(axisAV, axisBV));

		// plane's normal is directed as box2 --> box1
		if (XMVectorGetX(XMVector3Dot(normalV, centerOffsetV)) < 0)
			normalV = -normalV;

		// calculate edges' midpoints
		XMFLOAT3 midPointA = box1->GetHalfSize();
		XMFLOAT3 midPointB = box2->GetHalfSize();

		if (index1 == 0)
			midPointA.x = 0.0f;
		else if (index1 == 1)
			midPointA.y = 0.0f;
		else   // index1 == 2
			midPointA.z = 0.0f;

		if (index2 == 0)
			midPointB.x = 0.0f;
		else if (index2 == 1)
			midPointB.y = 0.0f;
		else   // index2 == 2
			midPointB.z = 0.0f;

		for (int i = 0; i < 3; i++)
		{
			if (index1 == i)
				continue;

			XMFLOAT3 axis = box1->GetAxis(i);
			
			if (XMVectorGetX(XMVector3Dot(normalV, XMLoadFloat3(&axis))) > 0)
			{
				if (i == 0)
					midPointA.x = -midPointA.x;
				else if (i == 1)
					midPointA.y = -midPointA.y;
				else   // i == 2
					midPointA.z = -midPointA.z;
			}
		}

		for (int i = 0; i < 3; i++)
		{
			if (index2 == i)
				continue;
			
			XMFLOAT3 axis = box2->GetAxis(i);
			
			if (XMVectorGetX(XMVector3Dot(normalV, XMLoadFloat3(&axis))) < 0)
			{
				if (i == 0)
					midPointB.x = -midPointB.x;
				else if (i == 1)
					midPointB.y = -midPointB.y;
				else   // i == 2
					midPointB.z = -midPointB.z;
			}
		}

		// get midpoints in world coordinates
		XMFLOAT4X4 box1WordlMatrix = box1->GetWorldMatrix();
		XMFLOAT4X4 box2WordlMatrix = box2->GetWorldMatrix();
		
		XMStoreFloat3(&midPointA, XMVector3Transform(XMLoadFloat3(&midPointA), XMLoadFloat4x4(&box1WordlMatrix)));
		XMStoreFloat3(&midPointB, XMVector3Transform(XMLoadFloat3(&midPointB), XMLoadFloat4x4(&box2WordlMatrix)));

		// calculate closest points on edges
		float modA = XMVectorGetX(XMVector3LengthSq(axisAV));
		float modB = XMVectorGetX(XMVector3LengthSq(axisBV));

		float dot = XMVectorGetX(XMVector3Dot(axisAV, axisBV));

		float den = dot * dot - modA * modB;

		XMVECTOR mid = XMLoadFloat3(&midPointA) - XMLoadFloat3(&midPointB);
		float projA = XMVectorGetX(XMVector3Dot(mid, axisAV));
		float projB = XMVectorGetX(XMVector3Dot(mid, axisBV));

		float la = (modB * projA - projB * dot) / den;
		float lb = (projA * dot - modA * projB) / den;

		XMVECTOR closestA = XMLoadFloat3(&midPointA) + axisAV * la;
		XMVECTOR closestB = XMLoadFloat3(&midPointB) + axisBV * lb;

		// contact point is the point halfway the edges' closest points
		XMStoreFloat3(&contactPoint, closestA / 2.0f + closestB / 2.0f);

		XMStoreFloat3(&contactNormal, normalV);
	}

	// store contact
	mContacts.InsertLast(new Contact(contactPoint, contactNormal, penetration, box1->GetOwner(), box2->GetOwner()));
}

void CollisionSystem::SphereAndSphereCollision(SphereCollisionComponent *sphere1, SphereCollisionComponent *sphere2)
{
	// get primitives' data
	XMFLOAT3 sphereCenter1 = sphere1->GetPosition();
	float sphereRadius1 = sphere1->GetRadius();

	XMFLOAT3 sphereCenter2 = sphere2->GetPosition();
	float sphereRadius2 = sphere2->GetRadius();

	// get distance between centers
	XMVECTOR centerOffsetV = XMLoadFloat3(&sphereCenter1) - XMLoadFloat3(&sphereCenter2);
	float distance = XMVectorGetX(XMVector3Length(centerOffsetV));

	// early out
	if (distance >= sphereRadius1 + sphereRadius2)
		return;

	// generate contact data
	XMFLOAT3 contactPoint;
	XMStoreFloat3(&contactPoint, XMLoadFloat3(&sphereCenter2) + centerOffsetV / 2.0f);

	XMFLOAT3 contactNormal;
	XMStoreFloat3(&contactNormal, XMVector3Normalize(centerOffsetV));  // contact normal is directed from primitive 2 to primitive 1

	float penetration = (sphereRadius1 + sphereRadius2) - distance;

	// add contact to list
	mContacts.InsertLast(new Contact(contactPoint, contactNormal, penetration, sphere1->GetOwner(), sphere2->GetOwner()));
}

void CollisionSystem::SphereAndHalfSpaceCollision(SphereCollisionComponent *sphere, PlaneCollisionComponent *plane)
{
	// get primitives' data
	XMFLOAT3 sphereCenter = sphere->GetPosition();
	float sphereRadius = sphere->GetRadius();

	XMFLOAT3 planeNormal = plane->GetNormal();
	float d = plane->GetOffset();

	// get distance between sphere's center and plane
	XMVECTOR planeNormalV = XMLoadFloat3(&planeNormal);
	float distance = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&sphereCenter), planeNormalV)) - d;

	// early out
	if (distance >= sphereRadius)
		return;

	// generate contact data
	XMFLOAT3 contactPoint;
	XMStoreFloat3(&contactPoint, XMLoadFloat3(&sphereCenter) - planeNormalV * (sphereRadius - (sphereRadius - distance) / 2.0f));

	XMFLOAT3 contactNormal;
	XMStoreFloat3(&contactNormal, planeNormalV);

	float penetration = sphereRadius - distance;

	// add contact to list
	mContacts.InsertLast(new Contact(contactPoint, contactNormal, penetration, sphere->GetOwner(), nullptr));
}

void CollisionSystem::SphereAndPlaneCollision(SphereCollisionComponent *sphere, PlaneCollisionComponent *plane)
{
	// get primitives' data
	XMFLOAT3 sphereCenter = sphere->GetPosition();
	float sphereRadius = sphere->GetRadius();

	XMFLOAT3 planeNormal = plane->GetNormal();
	float d = plane->GetOffset();

	// get distance between sphere's center and plane
	XMVECTOR planeNormalV = XMLoadFloat3(&planeNormal);
	float distance = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&sphereCenter), planeNormalV)) - d;

	// early out
	if (distance >= sphereRadius || -distance >= sphereRadius)
		return;

	// generate contact data
	if (distance < 0)
	{
		distance = -distance;
		planeNormalV = -planeNormalV;
	}

	XMFLOAT3 contactPoint;
	XMStoreFloat3(&contactPoint, XMLoadFloat3(&sphereCenter) - planeNormalV * (sphereRadius - (sphereRadius - distance / 2.0f)));
	
	XMFLOAT3 contactNormal;
	XMStoreFloat3(&contactNormal, planeNormalV);

	float penetration = sphereRadius - distance;

	// add contact to list
	mContacts.InsertLast(new Contact(contactPoint, contactNormal, penetration, sphere->GetOwner(), nullptr));
}

void CollisionSystem::BoxAndHalfSpaceCollision(BoxCollisionComponent *box, PlaneCollisionComponent *plane)
{
	// get primitives' data
	XMFLOAT3 boxHalfSize = box->GetHalfSize();
	XMFLOAT3 boxPosition = box->GetPosition();

	XMFLOAT3 planeNormal = plane->GetNormal();
	float d = plane->GetOffset();

	// get distance between box's center and plane
	XMVECTOR planeNormalV = XMLoadFloat3(&planeNormal);
	float distance = XMVectorGetX(XMVector3Dot(XMLoadFloat3(&boxPosition), planeNormalV)) - d;
	float boxRadius = XMVectorGetX(XMVector3Length(XMLoadFloat3(&boxHalfSize)));

	// early out
	if (boxRadius < distance)
		return;

	XMFLOAT3 boxVertices[8] =
	{
		XMFLOAT3(boxHalfSize.x, boxHalfSize.y, boxHalfSize.z),
		XMFLOAT3(boxHalfSize.x, -boxHalfSize.y, boxHalfSize.z),
		XMFLOAT3(-boxHalfSize.x, -boxHalfSize.y, boxHalfSize.z),
		XMFLOAT3(-boxHalfSize.x, boxHalfSize.y, boxHalfSize.z),
		XMFLOAT3(boxHalfSize.x, boxHalfSize.y, -boxHalfSize.z),
		XMFLOAT3(boxHalfSize.x, -boxHalfSize.y, -boxHalfSize.z),
		XMFLOAT3(-boxHalfSize.x, -boxHalfSize.y, -boxHalfSize.z),
		XMFLOAT3(-boxHalfSize.x, boxHalfSize.y, -boxHalfSize.z),
	};

	XMFLOAT4X4 worldMatrix = box->GetWorldMatrix();
	XMMATRIX worldMatrixM = XMLoadFloat4x4(&worldMatrix);

	// check for vertex-face contacts
	for (int i = 0; i < 8; i++)
	{
		XMVECTOR vertexWorldPositionV = XMVector3Transform(XMLoadFloat3(&boxVertices[i]), worldMatrixM);

		float distance = XMVectorGetX(XMVector3Dot(vertexWorldPositionV, planeNormalV)) - d;

		if (distance >= 0)
			continue;

		// generate contact data
		XMFLOAT3 contactPoint;
		XMStoreFloat3(&contactPoint, vertexWorldPositionV);

		XMFLOAT3 contactNormal;
		XMStoreFloat3(&contactNormal, planeNormalV);

		float penetration = -distance;

		// add contact to list
		mContacts.InsertLast(new Contact(contactPoint, contactNormal, penetration, box->GetOwner(), nullptr));
	}
}

void CollisionSystem::BoxAndSphereCollision(BoxCollisionComponent *box, SphereCollisionComponent *sphere)
{
	// get primitives' data
	XMFLOAT3 boxPosition = box->GetPosition();
	XMFLOAT3 boxHalfSize = box->GetHalfSize();

	XMFLOAT3 sphereCenter = sphere->GetPosition();
	float sphereRadius = sphere->GetRadius();

	// tranform sphere center into box local coordinates
	XMFLOAT4X4 boxInverseWorldMatrix = box->GetInverseWorldMatrix();
	XMVECTOR sphereCenterBoxV = XMVector3Transform(XMLoadFloat3(&sphereCenter), XMLoadFloat4x4(&boxInverseWorldMatrix));

	XMFLOAT3 sphereCenterBox;
	XMStoreFloat3(&sphereCenterBox, sphereCenterBoxV);

	// get sphere distance from box
	XMVECTOR boxHalfSizeV = XMLoadFloat3(&boxHalfSize);

	float boxRadius = XMVectorGetX(XMVector3Length(boxHalfSizeV));
	float distance = XMVectorGetX(XMVector3Length(sphereCenterBoxV));

	// early out - 1
	if (distance > boxRadius + sphereRadius)
		return;

	// get closest point on box to sphere (clamp to box halfsize)
	XMFLOAT3 boxClosestPoint = sphereCenterBox;

	if (sphereCenterBox.x > boxHalfSize.x)
		boxClosestPoint.x = boxHalfSize.x;
	else if (sphereCenterBox.x < -boxHalfSize.x)
		boxClosestPoint.x = -boxHalfSize.x;

	if (sphereCenterBox.y > boxHalfSize.y)
		boxClosestPoint.y = boxHalfSize.y;
	else if (sphereCenterBox.y < -boxHalfSize.y)
		boxClosestPoint.y = -boxHalfSize.y;

	if (sphereCenterBox.z > boxHalfSize.z)
		boxClosestPoint.z = boxHalfSize.z;
	else if (sphereCenterBox.z < -boxHalfSize.z)
		boxClosestPoint.z = -boxHalfSize.z;

	XMVECTOR boxClosestPointV = XMLoadFloat3(&boxClosestPoint);
	
	float closestDistance = XMVectorGetX(XMVector3Length(boxClosestPointV - sphereCenterBoxV));

	// early out - 2
	if (closestDistance > sphereRadius)
		return;

	// tranform closest point back to world coordinates
	XMFLOAT4X4 boxWorldMatrix = box->GetWorldMatrix();
	XMVECTOR boxClosestPointWorldV = XMVector3Transform(boxClosestPointV, XMLoadFloat4x4(&boxWorldMatrix));

	XMFLOAT3 boxClosestPointWorld;
	XMStoreFloat3(&boxClosestPointWorld, boxClosestPointWorldV);

	// generate contact data
	XMFLOAT3 contactPoint = boxClosestPointWorld;

	XMVECTOR sphereCenterV = XMLoadFloat3(&sphereCenter);
	XMVECTOR contactNormalV = XMVector3Normalize(boxClosestPointWorldV - sphereCenterV);

	XMFLOAT3 contactNormal;
	XMStoreFloat3(&contactNormal, contactNormalV);

	float penetration = sphereRadius - closestDistance;

	// add contact to list
	mContacts.InsertLast(new Contact(contactPoint, contactNormal, penetration, box->GetOwner(), sphere->GetOwner()));
}

#include "MotionComponent.h"

void CollisionSystem::RayAndSphereCollision(const XMFLOAT3 &ray, const XMFLOAT3 &origin, const SphereCollisionComponent *sphere)
{
	XMFLOAT3 sphereCenter = sphere->GetPosition();
	XMVECTOR sphereCenterV = XMLoadFloat3(&sphereCenter);
	float sphereRadius = sphere->GetRadius();

	XMVECTOR rayOriginV = XMLoadFloat3(&origin);
	XMVECTOR rayDirectionV = XMLoadFloat3(&ray);

	float a = 1.0f;
	float b = 2.0f * XMVectorGetX(XMVector3Dot((rayOriginV - sphereCenterV), rayDirectionV));
	float c = XMVectorGetX(XMVector3LengthSq(rayOriginV - sphereCenterV)) - sphereRadius * sphereRadius;
	float delta = b * b - 4.0f * a * c;

	if (delta < 0)  // no collision - early out
		return;

	if (delta == 0.0f)  // tangent point collision
	{

	}
	else  // delta > 0
	{
		float t1 = (-b - sqrt(delta)) / 2.0f;
		float t2 = (-b + sqrt(delta)) / 2.0f;

		// min t
		float t = t1 < t2 ? t1 : t2;

		if (t < 0.0f)   // intersection point is behind ray origin
			return;

		XMFLOAT3 contactPoint;
		XMStoreFloat3(&contactPoint, rayOriginV + t * rayDirectionV);

		XMFLOAT3 contactNormal;
		XMStoreFloat3(&contactNormal, rayDirectionV);

		XMFLOAT3 velocity;
		XMStoreFloat3(&velocity, rayDirectionV * 3.0f);

		sphere->GetOwner()->GetComponent<MotionComponent>()->AddVelocity(velocity);

		//float penetration = 0.1f;

		//mContacts.InsertLast(new Contact{ contactPoint, contactNormal, penetration, sphere->GetOwner(), nullptr });
	}
}