#ifndef RENDERING_SYSTEM_H
#define RENDERING_SYSTEM_H

#include "GUIRenderer.h"
#include "SkyBoxRenderer.h"
#include "StaticEntityRenderer.h"
//#include "TerrainRenderer.h"
#include "ShadowRenderer.h"

class Entity;

class RenderingSystem
{
public:
	static RenderingSystem &GetInstance() { static RenderingSystem instance; return instance; }

	void Render();
private:
	RenderingSystem() = default;

	GUIRenderer mGUIRenderer;
	SkyBoxRenderer mSkyBoxRenderer;
	StaticEntityRenderer mStaticEntityRenderer;
	//TerrainRenderer mTerrainRenderer;
	ShadowRenderer mShadowRenderer{ 1024 * 2, 768 * 2, 100.0f};
};

#endif  // RENDERING_SYSTEM_H