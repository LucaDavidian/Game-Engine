#include "TerrainGenerationStrategy.h"
#include "Terrain.h"
#include "StaticMeshComponent.h"
#include "Error.h"
#include <cstdio>

StaticMeshComponent *HeightMapTerrainGenerationStrategy::GenerateTerrain(Terrain *terrain)
{
	XMFLOAT3 position = terrain->GetPosition();
	float x0 = position.x;
	float y0 = position.y;
	float z0 = position.z;
	float width = terrain->GetWidth();
	float depth = terrain->GetDepth();
	unsigned int numNodesWidth = terrain->GetNumNodesWidth();
	unsigned int numNodesDepth = terrain->GetNumNodesDepth();

	unsigned int numVertices = numNodesWidth * numNodesDepth;

	float dx = width / (numNodesWidth - 1);
	float dz = depth / (numNodesDepth - 1);

	std::vector<float> &heights = terrain->GetHeights();

	std::vector<XMFLOAT3> positions;
	for (int i = 0; i < numNodesDepth; i++)
	{
		float z = z0 + depth / 2.0 - i * dz;

		for (int j = 0; j < numNodesWidth; j++)
		{
			float x = x0 - width / 2.0 + j * dx;

			// sample height map 
			float xHeightSample = j * dx / width;
			float zHeightSample = i * dz / depth;
			float height = y0 + SampleHeightMap(xHeightSample, zHeightSample, 6.0f);
			heights.push_back(height);
			positions.push_back(XMFLOAT3(x, height, z));
		}
	}

	std::vector<XMFLOAT2> textureCoordinates;
	for (int i = 0; i < numNodesDepth; i++)
		for (int j = 0; j < numNodesWidth; j++)
			textureCoordinates.push_back(XMFLOAT2((float)j / (float)(numNodesWidth - 1), (float)i / (float)(numNodesDepth - 1)));

	std::vector<unsigned int> indices;
	for (int i = 0; i < numNodesDepth - 1; i++)
		for (int j = 0; j < numNodesWidth - 1; j++)
		{
			indices.push_back(i * numNodesWidth + j);
			indices.push_back(i * numNodesWidth + j + 1);
			indices.push_back((i + 1) * numNodesWidth + j);

			indices.push_back((i + 1) * numNodesWidth + j);
			indices.push_back(i * numNodesWidth + j + 1);
			indices.push_back((i + 1) * numNodesWidth + j + 1);
		}

	std::vector<XMFLOAT3> normals;
	std::vector<std::vector<XMVECTOR>> accumulatedNormals(numVertices);
	for (int i = 0; i < indices.size(); i += 3)
	{
		XMFLOAT3 p0 = positions[indices[i]];
		XMFLOAT3 p1 = positions[indices[i + 1]];
		XMFLOAT3 p2 = positions[indices[i + 2]];

		XMVECTOR v1 = XMLoadFloat3(&p1) - XMLoadFloat3(&p0);
		XMVECTOR v2 = XMLoadFloat3(&p2) - XMLoadFloat3(&p0);

		XMVECTOR normal = XMVector3Normalize(XMVector3Cross(v1, v2));

		accumulatedNormals[indices[i]].push_back(normal);
		accumulatedNormals[indices[i + 1]].push_back(normal);
		accumulatedNormals[indices[i + 2]].push_back(normal);
	}
	for (const std::vector<XMVECTOR> &vertexNormals : accumulatedNormals)
	{
		XMVECTOR sum = XMVectorZero();
		int i = 0;
		for (XMVECTOR vertexNormal : vertexNormals)
			sum += vertexNormals[i++];
		sum /= XMVector3Length(sum);

		XMFLOAT3 normal;
		XMStoreFloat3(&normal, sum);
		normals.push_back(normal);
	}

	Mesh mesh;
	mesh.LoadAttribute("POSITION", &positions[0], positions.size());
	mesh.LoadAttribute("NORMAL", &normals[0], normals.size());
	mesh.LoadAttribute("TEX_COORD", &textureCoordinates[0], textureCoordinates.size());
	mesh.LoadIndexBuffer(indices);

	Material material;

	return new StaticMeshComponent(mesh, material, positions);
}

// load height map (bitmap format)
void HeightMapTerrainGenerationStrategy::LoadHeightMap(std::string const &heightMapFilePath)
{
	FILE *fileStream = nullptr;
	int e = fopen_s(&fileStream, heightMapFilePath.c_str(), "rb");
	if (e)
		ErrorBox("can't load height map");

	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;

	int read = fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, fileStream);
	if (read != 1)
		ErrorBox("can't load height map - file header");

	read = fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, fileStream);
	if (read != 1)
		ErrorBox("can't load height map - info header");

	mHeightMapWidth = infoHeader.biWidth;
	mHeightMapDepth = infoHeader.biHeight;

	std::vector<char> imageData(mHeightMapWidth * mHeightMapDepth * 3);

	fseek(fileStream, fileHeader.bfOffBits, SEEK_SET);
	read = fread(&imageData[0], 1, mHeightMapWidth * mHeightMapDepth * 3, fileStream);

	fclose(fileStream);

	for (int i = 0; i < mHeightMapWidth * mHeightMapWidth * 3; i += 3)
		mHeightMap.push_back(imageData[i]);
}

static float Lerp(float a, float b, float t)
{
	return (1 - t) * a + t * b;
}

// x and z sampling points between 0 and 1
float HeightMapTerrainGenerationStrategy::SampleHeightMap(float x, float z, float scaleFactor)
{
	float xSample = x * (mHeightMapWidth - 1);
	float zSample = z * (mHeightMapDepth - 1);
	int xInt = (int)xSample;
	int zInt = (int)zSample;
	float xFrac = xSample - xInt;
	float zFrac = zSample - zInt;

	int nextX = xInt >= mHeightMapWidth - 1 ? 0 : xInt + 1;
	int nextZ = zInt >= mHeightMapWidth - 1 ? 0 : zInt + 1;;

	unsigned char sample1 = mHeightMap[zInt * mHeightMapWidth + xInt];
	unsigned char sample2 = mHeightMap[zInt * mHeightMapWidth + nextX];
	float xValue1 = Lerp(sample1, sample2, xFrac);

	unsigned char sample3 = mHeightMap[nextZ * mHeightMapWidth + xInt];
	unsigned char sample4 = mHeightMap[nextZ * mHeightMapWidth + nextX];
	float xValue2 = Lerp(sample3, sample4, xFrac);

	return ((Lerp(xValue1, xValue2, zFrac)) - 128.0f) / scaleFactor;
}