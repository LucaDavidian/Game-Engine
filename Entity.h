#ifndef ENTITY_H
#define ENTITY_H

#include "utility/Bitset.h"
#include "utility/Utility.hpp"
#include "Component.h"

class Entity
{
public:
	Entity() : mIsAlive(true), mComponents{} { for (size_t i = 0; i < MAX_COMPONENTS; i++) mComponents[i] = nullptr; }
	~Entity() { for (Component *component : mComponents) delete component; }

	template <typename T, typename U = T, typename... Args>
	T &AddComponent(Args&&... args);
	template <typename T, typename U = T>
	T &AddComponent(T *component);
	template <typename T>
	bool HasComponent() const;
	template <typename T>
	T *GetComponent() const;

	bool IsAlive() const { return mIsAlive; }
	void Destroy() { mIsAlive = false; }
private:
	static const unsigned int MAX_COMPONENTS = 32;
	Component *mComponents[MAX_COMPONENTS];   // Entity owns components   TODO: multiple components
	Bitset<MAX_COMPONENTS> mComponentMask;

	bool mIsAlive;
};

template <typename T, typename U, typename... Args>
T &Entity::AddComponent(Args&&... args)
{
	static_assert(std::is_base_of<U,T>::value, "T is not a component derived from U");

	T *component = new T(utility::template forward<Args>(args)...);

	component->SetOwner(this);

	mComponents[GetComponentID<U>()] = component;  // TODO: multiple components
	
	if (!mComponentMask.Test(GetComponentID<U>()))
		mComponentMask.Set(GetComponentID<U>());

	component->Init();

	return *component;
}

template <typename T, typename U>
T &Entity::AddComponent(T *component)
{
	static_assert(std::is_base_of<U,T>::value, "T is not a component derived from U");

	mComponents[GetComponentID<U>()] = component;  // TODO: multiple components
	
	if (!mComponentMask.Test(GetComponentID<U>()))
		mComponentMask.Set(GetComponentID<U>());

	component->Init();

	return *component;
}

template <typename T>
bool Entity::HasComponent() const
{
	return mComponentMask[GetComponentID<T>()];
}

template <typename T>
T *Entity::GetComponent() const
{
	return static_cast<T*>(mComponents[GetComponentID<T>()]);
}

#endif  // ENTITY_H