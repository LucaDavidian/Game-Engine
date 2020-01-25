#include "TerrainGenerationStrategy.h"
#include "Terrain.h"
#include "StaticMeshComponent.h"

StaticMeshComponent *ProceduralTerrainGenerationStrategy::GenerateTerrain(Terrain *terrain)
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

			float y = y0 + ComputeHeight(x, z, 30.0f, 10.0f, 2.0f, 4);
			heights.push_back(y);
			positions.push_back(XMFLOAT3(x, y, z));
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

// compute vertex height using Perlin Noise algorithm
float ProceduralTerrainGenerationStrategy::ComputeHeight(float x, float z, float startFrequency, float startAmplitude, float persistence, int octaves)
{
	float frequency = startFrequency;
	float amplitude = startAmplitude;
	float value = 0.0f;
	for (int i = 0; i < octaves; i++)
	{
		//value += mNoiseGenerator.RandomNoiseTable(x / frequency, z / frequency) * amplitude;
		value += mNoiseGenerator.PerlinNoiseTable(x / frequency, z / frequency) * amplitude;
		frequency /= 2.0f;
		amplitude /= persistence;
	}

	return value;
}