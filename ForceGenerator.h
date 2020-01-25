#ifndef FORCE_GENERATOR_H
#define FORCE_GENERATOR_H

class Entity;

class ForceGenerator
{
public:
	virtual void AddForce(Entity *entity) = 0;
};

#endif  // FORCE_GENERATOR_H