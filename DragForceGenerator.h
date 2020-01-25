#ifndef DRAG_FORCE_GENERATOR_H
#define DRAG_FORCE_GENERATOR_H

#include "ForceGenerator.h"

class DragForceGenerator : public ForceGenerator
{
public:
	DragForceGenerator(float translationDrag, float rotationDrag) : mTranslationDrag(translationDrag), mRotationDrag(rotationDrag) {}
	void AddForce(Entity *entity) override;
private:
	float mTranslationDrag;
	float mRotationDrag;
};

#endif  // DRAG_FORCE_GENERATOR_H

