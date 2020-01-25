#ifndef SHADOW_RENDERER_H
#define SHADOW_RENDERER_H

#include "data structures/Vector.h"
#include "FrameBuffer.h"
#include "Texture.h"
#include "StaticShadowShader.h"
//#include "AnimatedShadowShader.h"

class Entity;

class ShadowRenderer
{
public:
	ShadowRenderer(int width, int height, float shadowDistance = 200.0f);

	void AddEntity(Entity *entity) { mEntities.InsertLast(entity); }

	void Render(Entity *camera, Entity *light, Entity *spotlight);

	const XMFLOAT4X4 &GetLightViewProjectionMatrix() const { return mLightViewProjectionMatrix; }
	Texture GetShadowMap() { return mShadowMap; }

	const XMFLOAT4X4 &GetLightViewProjectionMatrixSpot() const { return mLightViewProjectionMatrixSpot; }
	Texture GetShadowMapSpot() { return mShadowMapSpot; }

	float GetShadowDistance() const { return mShadowDistance; }
private:
	Vector<Entity*> mEntities;

	StaticShadowShader mShader;
	//AnimationShadowShader mAnimationShader;

	FrameBuffer mFrameBuffer;	
	FrameBuffer mFrameBufferSpot;
	Texture mShadowMap;
	Texture mShadowMapSpot;

	void ComputeLightViewProjection(Entity *camera, Entity *light);
	XMFLOAT4X4 mLightViewProjectionMatrix;
	XMFLOAT4X4 mLightViewProjectionMatrixSpot;
	
	float mShadowDistance;
};

#endif  //  SHADOW_RENDERER_H
