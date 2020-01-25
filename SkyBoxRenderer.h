#ifndef SKYBOX_RENDERER_H
#define SKYBOX_RENDERER_H

#include "SkyBoxShader.h"

class Entity;

class SkyBoxRenderer
{
public:
	void Render(Entity *skyBox, Entity *camera);
private:
	SkyBoxShader mShader;
};

#endif  // SKYBOX_RENDERER_H

