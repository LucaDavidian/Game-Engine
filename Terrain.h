#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <array>
#include <string>
#include "TerrainGenerationStrategy.h"

class StaticMeshComponent;
class Texture;

class Terrain
{
public:
	Terrain(float x0, float y0, float z0, float width, float depth, unsigned int numNodesWidth, unsigned int numNodesDepth, std::vector<Texture*> textures, TerrainGenerationStrategy *strategy);
	XMFLOAT3 GetPosition() const;
	float GetWidth() const;
	float GetDepth() const;
	unsigned int GetNumNodesWidth() const;
	unsigned int GetNumNodesDepth() const;
	StaticMeshComponent *GetModel();
	std::vector<float> &GetHeights();
	float GetHeight(float x, float z) const;
	std::vector<std::array<XMFLOAT3, 3>> GetTriangleSubset(float x, float z, float radius);
private:
	void GenerateTerrain();
	TerrainGenerationStrategy *mStrategy;
	float BarycentricInterpolation(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT2 pos) const;
	StaticMeshComponent *mModel;
	float mX0;
	float mY0;
	float mZ0;
	float mWidth;
	float mDepth;
	unsigned int mNumNodesWidth;
	unsigned int mNumNodesDepth;
	std::vector<float> mHeights;             // terrain's heights (noise or height map generated)
};

#endif  // TERRAIN_H

