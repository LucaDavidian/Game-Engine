#include <cstdio>
#include "Terrain.h"
#include "StaticMeshComponent.h"

Terrain::Terrain(float x0, float y0, float z0, float width, float depth, unsigned int numNodesWidth, unsigned int numNodesDepth, std::vector<Texture*> textures, TerrainGenerationStrategy *strategy)
	: mNumNodesWidth(numNodesWidth), mNumNodesDepth(numNodesDepth), mX0(x0), mY0(y0), mZ0(z0), mWidth(width), mDepth(depth), mStrategy(strategy)
{
	GenerateTerrain();

	std::vector<std::vector<Texture*>> &t = mModel->GetMaterial().GetDiffuseMap();
	std::vector<Texture*> &modelTextures = t[0];
	modelTextures.insert(modelTextures.begin(), textures.begin(), textures.end());
}

void Terrain::GenerateTerrain()
{
	mModel = mStrategy->GenerateTerrain(this);
}

XMFLOAT3 Terrain::GetPosition() const
{
	return XMFLOAT3(mX0, mY0, mZ0);
}

float Terrain::GetWidth() const
{
	return mWidth;
}

float Terrain::GetDepth() const
{
	return mDepth;
}

unsigned int Terrain::GetNumNodesWidth() const
{
	return mNumNodesWidth;
}

unsigned int Terrain::GetNumNodesDepth() const
{
	return mNumNodesDepth;
}

std::vector<float> &Terrain::GetHeights()
{
	return mHeights;
}

Model *Terrain::GetModel()
{
	return mModel;
}

float Terrain::BarycentricInterpolation(XMFLOAT3 p1, XMFLOAT3 p2, XMFLOAT3 p3, XMFLOAT2 pos) const
{
	float det = (p2.z - p3.z) * (p1.x - p3.x) + (p3.x - p2.x) * (p1.z - p3.z);
	float l1 = ((p2.z - p3.z) * (pos.x - p3.x) + (p3.x - p2.x) * (pos.y - p3.z)) / det;
	float l2 = ((p3.z - p1.z) * (pos.x - p3.x) + (p1.x - p3.x) * (pos.y - p3.z)) / det;
	float l3 = 1.0f - l1 - l2;
	return l1 * p1.y + l2 * p2.y + l3 * p3.y;
}

float Terrain::GetHeight(float x, float z) const
{
	// find grid cell (with respect to UPPER LEFT CORNER)
	float xGrid = x - (mX0 - mWidth / 2.0);
	float zGrid = -(z - (mZ0 + mDepth / 2.0));

	// if position is outside terrain return 0.0
	if (xGrid > mWidth || xGrid < 0.0f || zGrid > mDepth || zGrid < 0.0f)
		return 0.0f;

	float dx = mWidth / (mNumNodesWidth - 1);
	float dz = mDepth / (mNumNodesDepth - 1);

	xGrid /= dx;
	zGrid /= dz;

	// cell indices
	int j = (int)xGrid;
	int i = (int)zGrid;

	// position relative to UPPER LEFT CORNER of cell
	float relX = xGrid - j;
	float relZ = zGrid - i;

	// barycentric interpolation
	if (relZ >= 1 - relX)
	{
		XMFLOAT3 p1 = XMFLOAT3(1.0f, mHeights[i * mNumNodesWidth + j + 1], 0.0f);
		XMFLOAT3 p2 = XMFLOAT3(0.0f, mHeights[(i + 1) * mNumNodesWidth + j], 1.0f);
		XMFLOAT3 p3 = XMFLOAT3(1.0f, mHeights[(i + 1) * mNumNodesWidth + j + 1], 1.0f);
		XMFLOAT2 pos(relX, relZ);
		return BarycentricInterpolation(p1, p2, p3, pos);
	}
	else  // relZ < 1 - relX
	{
		XMFLOAT3 p1 = XMFLOAT3(0.0f, mHeights[i * mNumNodesWidth + j], 0.0f);
		XMFLOAT3 p2 = XMFLOAT3(1.0f, mHeights[i * mNumNodesWidth + j + 1], 0.0f);
		XMFLOAT3 p3 = XMFLOAT3(0.0f, mHeights[(i + 1) * mNumNodesWidth + j], 1.0f);
		XMFLOAT2 pos(relX, relZ);
		return BarycentricInterpolation(p1, p2, p3, pos);
	}
}

std::vector<std::array<XMFLOAT3, 3>> Terrain::GetTriangleSubset(float x, float z, float radius)
{
	// find grid cell (with respect to UPPER LEFT CORNER)
	float xOrigin = mX0 - mWidth / 2.0f;
	float zOrigin = mZ0 + mDepth / 2.0f;

	float xGrid = x - xOrigin;
	float zGrid = -(z - zOrigin);

	float dx = mWidth / (mNumNodesWidth - 1);
	float dz = mDepth / (mNumNodesDepth - 1);

	xGrid /= dx;
	zGrid /= dz;

	// cell indices
	int j = (int)xGrid;
	int i = (int)zGrid;

	int xRadius = 1 + radius / dx;
	int zRadius = 1 + radius / dz;

	int iStart = i - zRadius;
	if (iStart < 0)
		iStart = 0;

	int iEnd = i + zRadius;
	if (iEnd >= mNumNodesDepth)
		iEnd = mNumNodesDepth - 1;

	int jStart = j - xRadius;
	if (jStart < 0)
		jStart = 0;

	int jEnd = j + xRadius;
	if (jEnd >= mNumNodesWidth)
		jEnd = mNumNodesWidth - 1;

	std::vector<std::array<XMFLOAT3, 3>> triangles;
	
	for (int i = iStart; i <= iEnd; i++)
		for (int j = jStart; j <= jEnd; j++)
		{
			XMFLOAT3 p1 = XMFLOAT3(xOrigin + j * dx, mHeights[i * mNumNodesWidth + j], zOrigin - i * dz);
			XMFLOAT3 p2 = XMFLOAT3(xOrigin + (j + 1) * dx, mHeights[i * mNumNodesWidth + j + 1], zOrigin - i * dz);
			XMFLOAT3 p3 = XMFLOAT3(xOrigin + j * dx, mHeights[(i + 1) * mNumNodesWidth + j], zOrigin - (i + 1) * dz);

			std::array<XMFLOAT3, 3> triangle1 = { p1, p2, p3 };
			triangles.push_back(triangle1);

			XMFLOAT3 p4 = XMFLOAT3(xOrigin + j * dx, mHeights[(i + 1) * mNumNodesWidth + j], zOrigin - (i + 1) * dz);
			XMFLOAT3 p5 = XMFLOAT3(xOrigin + (j + 1) * dx, mHeights[i * mNumNodesWidth + j + 1], zOrigin - i * dz);
			XMFLOAT3 p6 = XMFLOAT3(xOrigin + (j + 1) * dx, mHeights[(i + 1) * mNumNodesWidth + j + 1], zOrigin - (i + 1) * dz);

			std::array<XMFLOAT3, 3> triangle2 = { p4, p5, p6 };
			triangles.push_back(triangle2);
		}

	return triangles;
}


