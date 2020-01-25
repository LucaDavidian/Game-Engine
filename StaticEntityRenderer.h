#ifndef STATIC_ENTITY_RENDERER_H
#define STATIC_ENTITY_RENDERER_H

#include "data structures/Vector.h"
#include "StaticEntityShader.h"

class Entity;
class Texture;

class StaticEntityRenderer
{
public:
	void Render(Entity *camera, Vector<Entity*> const &lights, Texture shadowMap, Texture shadowMapSpot, XMFLOAT4X4 const &lightViewProjectionMatrix, XMFLOAT4X4 const &lightViewProjectionMatrixSpot, float shadowDistance);
	void AddEntity(Entity *entity) { mEntities.InsertLast(entity); }
private:
	StaticEntityShader mShader;
	Vector<Entity*> mEntities;
};

#endif  // STATIC_ENTITY_RENDERER_H

