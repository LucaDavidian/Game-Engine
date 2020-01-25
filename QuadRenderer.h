#ifndef QUADRENDERER_H
#define QUADRENDERER_H

#include "QuadShader.h"
#include "Mesh.h"
#include <vector>

class Texture;

struct Quad
{
	Quad(const XMFLOAT2 &position, const XMFLOAT2 &dimensions, const XMFLOAT3 &color, float opacity, Texture *texture) : mPosition(position), mDimensions(dimensions), mColor(color), mOpacity(opacity), mTexture(texture) {}
	XMFLOAT2 mPosition;
	XMFLOAT2 mDimensions;
	XMFLOAT3 mColor;
	float mOpacity;
	Texture *mTexture;
};

class QuadRenderer
{
public:
	static QuadRenderer &GetInstance() { static QuadRenderer instance; return instance; }
	void AddQuad(const XMFLOAT2 &position, const XMFLOAT2 &dimensions, const XMFLOAT3 &color, float opacity, Texture *texture);
	void Render();
private:
	QuadRenderer();
	Mesh mMesh;
	QuadShader mShader;
	std::vector<Quad> mQuads;
};

#endif  // QUADRENDERER_H