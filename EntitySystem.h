#ifndef ENTITY_SYSTEM_H
#define ENTITY_SYSTEM_H

#include "data structures/Vector.h"
#include "Entity.h"

class EntitySystem
{
public:
	static EntitySystem &GetInstance() { static EntitySystem instance; return instance; }
	Entity &AddEntity() { Entity *entity = new Entity; mEntities.InsertLast(entity); return *entity; }
	const Vector<Entity*> &GetEntities() const { return mEntities; }
	void Update();
	Entity *GetCamera() const { return mCamera; }
	void SetCamera(Entity *entity) { mCamera = entity; }
private:
	EntitySystem() = default;

	Vector<Entity*> mEntities;  // EntitySystem owns entities
	Entity *mCamera;
};

#endif  // ENTITY_SYSTEM_H