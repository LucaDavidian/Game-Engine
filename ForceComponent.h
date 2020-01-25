#ifndef FORCE_COMPONENT_H
#define FORCE_COMPONENT_H

#include "Component.h"
#include "ForceGenerator.h"
#include "data structures/Vector.h"

class ForceComponent : public Component
{
public:
	~ForceComponent() { for (ForceGenerator *forceGenerator : mForceGenerators) delete forceGenerator; }

	void AddForceGenerator(ForceGenerator *forceGenerator) { mForceGenerators.InsertLast(forceGenerator); }
	void UpdateForce();
	void ClearAccumulators();
private:
	Vector<ForceGenerator*> mForceGenerators;
};

#endif  // FORCE_COMPONENT_H

