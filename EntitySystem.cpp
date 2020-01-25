#include "EntitySystem.h"
#include <algorithm>

void EntitySystem::Update()
{
	if (!mEntities.Size())
		return;

	// remove dead entities
	Vector<Entity*>::Iterator it = std::remove_if(mEntities.begin(), mEntities.end(), [](Entity *entity) { return !entity->IsAlive(); });
	
	auto it2 = it;

	while (it2 != mEntities.end())
		delete *it2++;

	mEntities.Remove(it, mEntities.end());
}
