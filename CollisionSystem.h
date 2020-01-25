#ifndef COLLISION_SYSTEM_H
#define COLLISION_SYSTEM_H

#include "data structures/Vector.h"
#include "Contact.h"

class BoxCollisionComponent;
class SphereCollisionComponent;
class PlaneCollisionComponent;

class CollisionSystem
{
public:
	static CollisionSystem &GetInstance() { static CollisionSystem instance; return instance; }

	void AddRay(class Picker *picker) { mPicker = picker; }

	void DoCollisions();
private:
	CollisionSystem() = default;

	// separating axis theorem
	Vector<XMFLOAT3> GetSATAxes(BoxCollisionComponent *box1, BoxCollisionComponent *box2);
	float PerformSAT(const XMFLOAT3 &axis, BoxCollisionComponent *box1, BoxCollisionComponent *box2);

	// collision detection - contact data generation algoritms	
	void BoxAndBoxCollision(BoxCollisionComponent *box1,BoxCollisionComponent *box2);
	void BoxAndHalfSpaceCollision(BoxCollisionComponent *box, PlaneCollisionComponent *plane);
	void BoxAndSphereCollision(BoxCollisionComponent *box, SphereCollisionComponent *sphere);
	
	void SphereAndSphereCollision(SphereCollisionComponent *s1, SphereCollisionComponent *s2);
	void SphereAndHalfSpaceCollision(SphereCollisionComponent *sphere, PlaneCollisionComponent *plane);
	void SphereAndPlaneCollision(SphereCollisionComponent *sphere, PlaneCollisionComponent *plane);

	void RayAndSphereCollision(const XMFLOAT3 &ray, const XMFLOAT3 &origin, const SphereCollisionComponent *sphere);
	void RayAndBoxCollision(const XMFLOAT3 &ray, const XMFLOAT3 &origin, const BoxCollisionComponent *box);
	void RayAndPlaneCollision(const XMFLOAT3 &ray, const XMFLOAT3 &origin, const PlaneCollisionComponent *sphere);

	void ResolveContacts();

	Vector<Contact*> mContacts;

	Picker *mPicker;
};

#endif  // COLLISION_SYSTEM_H
