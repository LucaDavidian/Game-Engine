#ifndef COMPONENT_H
#define COMPONENT_H

#include <type_traits>

unsigned int GetUniqueID();

class Component;

template <typename T>
unsigned int GetComponentID()
{
	static_assert(std::is_base_of<Component,T>::value, "T is not a component type");

	static unsigned int componentID = GetUniqueID();

	return componentID;
}

class Entity;

class Component
{
public:
	virtual ~Component() = default;

	virtual void Init() {}

	void SetOwner(Entity *entity) { mEntity = entity;  }
	Entity *GetOwner() const { return mEntity; }
protected:
	Component() : mEntity(nullptr) {}
private:
	Entity *mEntity;
};

#endif  // COMPONENT_H