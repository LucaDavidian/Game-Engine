#ifndef TERRAIN_RENDERER_H
#define TERRAIN_RENDERER_H

#include "data structures/Vector.h"
#include "TerrainShader.h"

class Entity;
class Texture;

class TerrainRenderer
{
public:
	void Render(Entity *camera, Vector<Entity*> const &lights, Texture shadowMap, Texture shadowMapSpot, XMFLOAT4X4 const &lightViewProjectionMatrix, XMFLOAT4X4 const &lightViewProjectionMatrixSpot, float shadowDistance, const XMFLOAT4 &clipPlane);
private:
	TerrainShader mShader;
};

#endif  // TERRAIN_RENDERER_H

