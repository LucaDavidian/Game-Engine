#include "GeometryGenerator.h"
#include <vector>
#include "StaticMeshComponent.h"
#include "Mesh.h"
#include "Material.h"

StaticMeshComponent *GeometryGenerator::GenerateSkybox(const std::string &cubeMap)
{
	std::vector<XMFLOAT3> positions;

	positions.push_back(XMFLOAT3(-0.5f, 0.5f, -0.5f));
	positions.push_back(XMFLOAT3(0.5f, 0.5f, -0.5f));
	positions.push_back(XMFLOAT3(0.5f, -0.5f, -0.5f));
	positions.push_back(XMFLOAT3(-0.5f, -0.5f, -0.5f));
	positions.push_back(XMFLOAT3(-0.5f, 0.5f, 0.5f));
	positions.push_back(XMFLOAT3(0.5f, 0.5f, 0.5f));
	positions.push_back(XMFLOAT3(0.5f, -0.5f, 0.5f));
	positions.push_back(XMFLOAT3(-0.5f, -0.5f, 0.5f));

	std::vector<unsigned int> indices{ 0, 3, 2, 2, 1, 0, 4, 5, 6, 6, 7, 4, 1, 2, 6, 6, 5, 1, 0, 4, 7, 7, 3, 0, 4, 0, 1, 1, 5, 4, 3, 7, 6, 6, 2, 3 };

	Mesh mesh;
	mesh.LoadAttribute("POSITION", &positions[0], positions.size());
	mesh.LoadIndexBuffer(indices);

	Material material;
	material.AddDiffuseMap(cubeMap);

	return new StaticMeshComponent(std::vector<Mesh>{mesh}, std::vector<Material>{material}, positions);
}

StaticMeshComponent *GeometryGenerator::GenerateSphere(float radius, Material material)
{
	int numStacks = 40;
	int numSlices = 40;

	float thetaStep = XM_PI / numStacks;
	float phiStep = 2.0f * XM_PI / numSlices;

	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;
	std::vector<XMFLOAT3> tangents;
	std::vector<XMFLOAT2> textureCoordinates;

	positions.push_back(XMFLOAT3(0.0f, radius, 0.0f));   // positions[0] is top pole
	normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
	textureCoordinates.push_back(XMFLOAT2(0.0f, 0.0f));

	int numRings = numStacks + 1;

	for (int i = 1; i < numRings - 1; i++)
	{
		float theta = i * thetaStep;

		for (int j = 0; j <= numSlices; j++)
		{
			float phi = j * phiStep;

			XMFLOAT3 position;
			position.x = radius * sin(theta) * cos(phi);
			position.y = radius * cos(theta);
			position.z = radius * sin(theta) * sin(phi);

			positions.push_back(position);

			XMFLOAT3 normal;
			XMStoreFloat3(&normal, XMVector3Normalize(XMLoadFloat3(&position)));

			normals.push_back(normal);

			textureCoordinates.push_back(XMFLOAT2(phi / (2.0f * XM_PI), theta / (XM_PI)));
		}
	}

	positions.push_back(XMFLOAT3(0.0f, -radius, 0.0f));   // positions[positions.size() - 1] is bottom pole
	normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
	textureCoordinates.push_back(XMFLOAT2(0.0f, 1.0f));

	// indices
	std::vector<unsigned int> indices;

	unsigned int firstStackOffset = 1;

	/* first stack (top pole) */
	for (int i = 0; i < numSlices; i++)
	{
		indices.push_back(0);
		indices.push_back(firstStackOffset + i + 1);
		indices.push_back(firstStackOffset + i);
	}

	int ringVertexCount = numSlices + 1;

	/* n - 2 stacks */
	for (int i = 0; i < numStacks - 2; i++)
		for (int j = 0; j < numSlices; j++)
		{
			indices.push_back(firstStackOffset + i * ringVertexCount + j);
			indices.push_back(firstStackOffset + i * ringVertexCount + (j + 1));
			indices.push_back(firstStackOffset + (i + 1) * ringVertexCount + j);

			indices.push_back(firstStackOffset + (i + 1) * ringVertexCount + j);
			indices.push_back(firstStackOffset + i * ringVertexCount + (j + 1));
			indices.push_back(firstStackOffset + (i + 1) * ringVertexCount + (j + 1));
		}

	int southPoleIndex = positions.size() - 1;
	int baseIndex = southPoleIndex - ringVertexCount;

	/* last stack (bottom pole) */
	for (int i = 0; i < numSlices; ++i)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + (i + 1));
	}

	// calculate tangents
	std::vector<std::vector<XMFLOAT3>> accumulatedTangents(positions.size());

	for (int i = 0; i < indices.size(); i += 3)
	{
		XMFLOAT3 p0 = positions[indices[i]];
		XMFLOAT3 p1 = positions[indices[i + 1]];
		XMFLOAT3 p2 = positions[indices[i + 2]];

		XMVECTOR v1 = XMLoadFloat3(&p1) - XMLoadFloat3(&p0);
		XMVECTOR v2 = XMLoadFloat3(&p2) - XMLoadFloat3(&p0);

		float du1 = textureCoordinates[indices[i + 1]].x - textureCoordinates[indices[i]].x;
		float dv1 = textureCoordinates[indices[i + 1]].y - textureCoordinates[indices[i]].y;
		float du2 = textureCoordinates[indices[i + 2]].x - textureCoordinates[indices[i]].x;
		float dv2 = textureCoordinates[indices[i + 2]].y - textureCoordinates[indices[i]].y;

		float det = du1 * dv2 - dv1 * du2;

		XMFLOAT3 triangleTangent;
		XMStoreFloat3(&triangleTangent, (dv2 * v1 - dv1 * v2) / det);

		accumulatedTangents[indices[i]].push_back(triangleTangent);
		accumulatedTangents[indices[i + 1]].push_back(triangleTangent);
		accumulatedTangents[indices[i + 2]].push_back(triangleTangent);
	}

	// average vertex tangents
	for (std::vector<XMFLOAT3> vertexTangents : accumulatedTangents)
	{
		XMVECTOR sum = XMVectorZero();

		for (XMFLOAT3 vertexTangent : vertexTangents)
			sum += XMLoadFloat3(&vertexTangent);

		XMFLOAT3 averagedTangent;
		XMStoreFloat3(&averagedTangent, sum / XMVector3Length(sum));

		tangents.push_back(averagedTangent);
	}

	Mesh mesh;
	mesh.LoadAttribute("POSITION", &positions[0], positions.size());
	mesh.LoadAttribute("NORMAL", &normals[0], normals.size());
	mesh.LoadAttribute("TANGENT", &tangents[0], tangents.size());
	mesh.LoadAttribute("TEX_COORD", &textureCoordinates[0], textureCoordinates.size());
	mesh.LoadIndexBuffer(indices);

	return new StaticMeshComponent(std::vector<Mesh>{mesh}, std::vector<Material>{material}, positions);
}

StaticMeshComponent *GeometryGenerator::GenerateBox(XMFLOAT3 halfsize, Material material)
{
	std::vector<XMFLOAT3> positions =
	{
		XMFLOAT3(halfsize.x, halfsize.y, -halfsize.z), XMFLOAT3(halfsize.x, -halfsize.y, -halfsize.z), XMFLOAT3(-halfsize.x, -halfsize.y, -halfsize.z),
		XMFLOAT3(-halfsize.x, -halfsize.y, -halfsize.z), XMFLOAT3(-halfsize.x, halfsize.y, -halfsize.z), XMFLOAT3(halfsize.x, halfsize.y, -halfsize.z),

		XMFLOAT3(-halfsize.x, -halfsize.y, halfsize.z), XMFLOAT3(halfsize.x, -halfsize.y, halfsize.z), XMFLOAT3(halfsize.x, halfsize.y, halfsize.z),
		XMFLOAT3(halfsize.x, halfsize.y, halfsize.z), XMFLOAT3(-halfsize.x, halfsize.y, halfsize.z), XMFLOAT3(-halfsize.x, -halfsize.y, halfsize.z),

		XMFLOAT3(halfsize.x, halfsize.y, halfsize.z), XMFLOAT3(halfsize.x, -halfsize.y, halfsize.z), XMFLOAT3(halfsize.x, -halfsize.y, -halfsize.z),
		XMFLOAT3(halfsize.x, -halfsize.y, -halfsize.z), XMFLOAT3(halfsize.x, halfsize.y, -halfsize.z), XMFLOAT3(halfsize.x, halfsize.y, halfsize.z),

		XMFLOAT3(-halfsize.x, -halfsize.y, -halfsize.z), XMFLOAT3(-halfsize.x, -halfsize.y, halfsize.z), XMFLOAT3(-halfsize.x, halfsize.y, halfsize.z),
		XMFLOAT3(-halfsize.x, halfsize.y, halfsize.z), XMFLOAT3(-halfsize.x, halfsize.y, -halfsize.z), XMFLOAT3(-halfsize.x, -halfsize.y, -halfsize.z),

		XMFLOAT3(-halfsize.x, -halfsize.y, -halfsize.z), XMFLOAT3(halfsize.x, -halfsize.y, -halfsize.z), XMFLOAT3(halfsize.x, -halfsize.y, halfsize.z),
		XMFLOAT3(halfsize.x, -halfsize.y, halfsize.z), XMFLOAT3(-halfsize.x, -halfsize.y, halfsize.z), XMFLOAT3(-halfsize.x, -halfsize.y, -halfsize.z),

		XMFLOAT3(halfsize.x, halfsize.y, halfsize.z), XMFLOAT3(halfsize.x, halfsize.y, -halfsize.z), XMFLOAT3(-halfsize.x, halfsize.y, -halfsize.z),
		XMFLOAT3(-halfsize.x, halfsize.y, -halfsize.z), XMFLOAT3(-halfsize.x, halfsize.y, halfsize.z),XMFLOAT3(halfsize.x, halfsize.y, halfsize.z),
	};

	std::vector<XMFLOAT3> normals =
	{
		XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),
		XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f), XMFLOAT3(0.0f, 0.0f, -1.0f),

		XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),
		XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f), XMFLOAT3(0.0f, 0.0f, 1.0f),

		XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),
		XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f),

		XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),
		XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f), XMFLOAT3(-1.0f, 0.0f, 0.0f),

		XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),
		XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f), XMFLOAT3(0.0f, -1.0f, 0.0f),

		XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f),
	};

	std::vector<XMFLOAT2> textureCoordinates =
	{
		XMFLOAT2(0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f),

		XMFLOAT2(1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f),

		XMFLOAT2(0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f),

		XMFLOAT2(1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f),

		XMFLOAT2(0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f),
		XMFLOAT2(1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f), XMFLOAT2(0.0f, 1.0f),

		XMFLOAT2(1.0f, 1.0f), XMFLOAT2(0.0f, 1.0f), XMFLOAT2(0.0f, 0.0f),
		XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT2(1.0f, 1.0f),
	};

	std::vector<unsigned int> indices{ 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35 };

	// calculate tangents
	std::vector<XMFLOAT3> tangents;
	std::vector<std::vector<XMFLOAT3>> accumulatedTangents(positions.size());

	for (int i = 0; i < indices.size(); i += 3)
	{
		XMFLOAT3 p0 = positions[indices[i]];
		XMFLOAT3 p1 = positions[indices[i + 1]];
		XMFLOAT3 p2 = positions[indices[i + 2]];

		XMVECTOR v1 = XMLoadFloat3(&p1) - XMLoadFloat3(&p0);
		XMVECTOR v2 = XMLoadFloat3(&p2) - XMLoadFloat3(&p0);

		float du1 = textureCoordinates[indices[i + 1]].x - textureCoordinates[indices[i]].x;
		float dv1 = textureCoordinates[indices[i + 1]].y - textureCoordinates[indices[i]].y;
		float du2 = textureCoordinates[indices[i + 2]].x - textureCoordinates[indices[i]].x;
		float dv2 = textureCoordinates[indices[i + 2]].y - textureCoordinates[indices[i]].y;

		float det = du1 * dv2 - dv1 * du2;

		XMFLOAT3 triangleTangent;
		XMStoreFloat3(&triangleTangent, (dv2 * v1 - dv1 * v2) / det);

		accumulatedTangents[indices[i]].push_back(triangleTangent);
		accumulatedTangents[indices[i + 1]].push_back(triangleTangent);
		accumulatedTangents[indices[i + 2]].push_back(triangleTangent);
	}

	// average vertex tangents
	for (std::vector<XMFLOAT3> vertexTangents : accumulatedTangents)
	{
		XMVECTOR sum = XMVectorZero();

		for (XMFLOAT3 vertexTangent : vertexTangents)
			sum += XMLoadFloat3(&vertexTangent);

		XMFLOAT3 averagedTangent;
		XMStoreFloat3(&averagedTangent, sum / XMVector3Length(sum));

		tangents.push_back(averagedTangent);
	}

	Mesh mesh;
	mesh.LoadAttribute("POSITION", &positions[0], 36);
	mesh.LoadAttribute("NORMAL", &normals[0], 36);
	mesh.LoadAttribute("TANGENT", &tangents[0], 36);
	mesh.LoadAttribute("TEX_COORD", &textureCoordinates[0], 36);
	mesh.SetVertexCount(36);

	return new StaticMeshComponent(std::vector<Mesh>{mesh}, std::vector<Material> {material}, positions);
}

StaticMeshComponent *GeometryGenerator::GeneratePlane(float width, float depth, Material material)
{
	std::vector<XMFLOAT3> positions =
	{
		XMFLOAT3(-width / 2.0f, 0.0f, -depth / 2.0f), XMFLOAT3(width / 2.0f, 0.0f, -depth / 2.0f), XMFLOAT3(width / 2.0f, 0.0f, depth / 2.0f), XMFLOAT3(-width / 2.0f, 0.0f, depth / 2.0f)
	};

	std::vector<XMFLOAT3> normals =
	{
		XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f), XMFLOAT3(0.0f, 1.0f, 0.0f)
	};

	std::vector<XMFLOAT3> tangents =
	{
		XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f), XMFLOAT3(1.0f, 0.0f, 0.0f)
	};

	std::vector<XMFLOAT2> textureCoordinates =
	{
		XMFLOAT2(0.0f, 1.0f), XMFLOAT2(1.0f, 1.0f), XMFLOAT2(1.0f, 0.0f), XMFLOAT2(0.0f, 0.0f)
	};

	Mesh mesh;
	mesh.LoadAttribute("POSITION", &positions[0], 4);
	mesh.LoadAttribute("NORMAL", &normals[0], 4);
	mesh.LoadAttribute("TANGENT", &tangents[0], 4);
	mesh.LoadAttribute("TEX_COORD", &textureCoordinates[0], 4);
	mesh.LoadIndexBuffer({ 0, 3, 2, 2, 1, 0 });

	return new StaticMeshComponent(std::vector<Mesh>{mesh}, std::vector<Material>{material}, positions);
}

StaticMeshComponent *GeometryGenerator::GenerateCylinder(float bottomRadius, float topRadius, float height, Material material)
{
	int slices = 40;
	float thetaStep = 2 * XM_PI / slices;

	int stacks = 15;
	float stackHeight = height / stacks;
	float radiusStep = (topRadius - bottomRadius) / stacks;

	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;

	positions.push_back(XMFLOAT3(0.0f, -height / 2.0f, 0.0f));   // positions[0] is center of bottom base
	positions.push_back(XMFLOAT3(0.0f, height / 2.0f, 0.0f));    // positions[1] is center of top base

	normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
	normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));

	unsigned int lateralSurfaceOffset = positions.size();

	/* lateral surface vertices */
	for (int i = 0; i < stacks + 1; i++)
	{
		float radius = bottomRadius + radiusStep * i;
		float y = -height / 2.0f + stackHeight * i;

		for (int j = 0; j <= slices; j++)
		{
			float theta = thetaStep * j;

			float x = radius * cos(theta);
			float z = radius * sin(theta);

			positions.push_back(XMFLOAT3(x, y, z));

			XMVECTOR tangent = XMVectorSet(-radius * sin(theta), 0.0f, radius * cos(theta), 0.0f);
			tangent = XMVector3Normalize(tangent);

			XMVECTOR bitangent = XMVectorSet(((topRadius - bottomRadius) / 2.0f) * cos(theta), 1.0f, ((topRadius - bottomRadius) / 2.0f) * sin(theta), 0.0f);
			bitangent = XMVector3Normalize(bitangent);

			XMVECTOR normal = -XMVector3Normalize(XMVector3Cross(tangent, bitangent));

			float nx = XMVectorGetX(normal);
			float ny = XMVectorGetY(normal);
			float nz = XMVectorGetZ(normal);

			normals.push_back(XMFLOAT3(nx, ny, nz));
		}
	}

	unsigned int bottomBaseOffset = positions.size();

	/* bottom base vertices */
	for (int j = 0; j <= slices; j++)
	{
		float theta = thetaStep * j;

		float x = bottomRadius * cos(theta);
		float z = bottomRadius * sin(theta);

		positions.push_back(XMFLOAT3(x, -height / 2.0f, z));

		normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
	}

	unsigned int topBaseOffset = positions.size();

	/* top base vertices */
	for (int j = 0; j <= slices; j++)
	{
		float theta = thetaStep * j;

		float x = topRadius * cos(theta);
		float z = topRadius * sin(theta);

		positions.push_back(XMFLOAT3(x, height / 2.0f, z));

		normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));
	}

	std::vector<unsigned int> indices;

	/* lateral surface indices */
	for (int i = 0; i < stacks; i++)
		for (int j = 0; j < slices; j++)
		{
			indices.push_back(lateralSurfaceOffset + j + i * (slices + 1));
			indices.push_back(lateralSurfaceOffset + j + (i + 1) * (slices + 1));
			indices.push_back(lateralSurfaceOffset + (j + 1) + (i + 1) * (slices + 1));

			indices.push_back(lateralSurfaceOffset + (j + 1) + (i + 1) * (slices + 1));
			indices.push_back(lateralSurfaceOffset + (j + 1) + i * (slices + 1));
			indices.push_back(lateralSurfaceOffset + j + i * (slices + 1));
		}

	/* bottom base indices */
	for (int i = 0; i < slices; i++)
	{
		indices.push_back(0);
		indices.push_back(bottomBaseOffset + i);
		indices.push_back(bottomBaseOffset + i + 1);
	}

	/* top base indices */
	for (int i = 0; i < slices; i++)
	{
		indices.push_back(1);
		indices.push_back(topBaseOffset + i + 1);
		indices.push_back(topBaseOffset + i);
	}

	std::vector<XMFLOAT3> tangents(positions.size());
	std::vector<XMFLOAT2> textureCoordinates(positions.size());

	Mesh mesh;
	mesh.LoadAttribute("POSITION", &positions[0], positions.size());
	mesh.LoadAttribute("NORMAL", &normals[0], normals.size());
	mesh.LoadAttribute("TANGENT", &tangents[0], tangents.size());
	mesh.LoadAttribute("TEX_COORD", &textureCoordinates[0], textureCoordinates.size());
	mesh.LoadIndexBuffer(indices);

	return new StaticMeshComponent(std::vector<Mesh>{mesh}, std::vector<Material>{material}, positions);
}

StaticMeshComponent *GeometryGenerator::GenerateCone(float radius, float height, Material material)
{
	int slices = 50;
	float thetaStep = 2 * XM_PI / slices;

	std::vector<XMFLOAT3> positions;
	std::vector<XMFLOAT3> normals;

	positions.push_back(XMFLOAT3(0.0f, 0.0f, 0.0f));    // positions[0] is center of circular base
	positions.push_back(XMFLOAT3(0.0f, height, 0.0f));  // positions[1] is apex of cone

	normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
	normals.push_back(XMFLOAT3(0.0f, 1.0f, 0.0f));

	/* circular base vertices (base) */
	for (int i = 0; i <= slices; i++)
	{
		float theta = i * thetaStep;
		float x = radius * cos(theta);
		float z = radius * sin(theta);

		positions.push_back(XMFLOAT3(x, 0.0f, z));
		normals.push_back(XMFLOAT3(0.0f, -1.0f, 0.0f));
	}

	unsigned int indexOffset = positions.size();

	/* circular base vertices duplicates (lateral surface) */
	for (int i = 0; i <= slices; i++)
	{
		float theta = i * thetaStep;
		float x = radius * cos(theta);
		float z = radius * sin(theta);

		positions.push_back(XMFLOAT3(x, 0.0f, z));

		XMVECTOR tangent = XMVectorSet(-radius * sin(theta), 0.0f, radius * cos(theta), 0.0f);
		tangent = XMVector3Normalize(tangent);

		XMVECTOR bitangent = XMVectorSet((-radius/height) * cos(theta), 1.0f, (-radius/height) * sin(theta), 0.0f);
		bitangent = XMVector3Normalize(bitangent);

		XMVECTOR normal = -XMVector3Normalize(XMVector3Cross(tangent, bitangent));

		float nx = XMVectorGetX(normal);
		float ny = XMVectorGetY(normal);
		float nz = XMVectorGetZ(normal);
		
		normals.push_back(XMFLOAT3(nx, ny, nz));
	}

	std::vector<unsigned int> indices;

	/* circular base indices */
	for (int i = 0; i < slices; i++)
	{
		unsigned int indexBase = 2;

		indices.push_back(0);
		indices.push_back(indexBase + i);
		indices.push_back(indexBase + i + 1);
		//indices.push_back((indexBase + i + 1) % (slices + indexBase) ? indexBase + i + 1 : indexBase);
	}

	/* lateral surface indices */
	for (int i = 0; i < slices; i++)
	{
		unsigned int indexBase = indexOffset;

		indices.push_back(1);
		indices.push_back(indexBase + i + 1);
		//indices.push_back((indexBase + i + 1) % (slices + indexBase) ? indexBase + i + 1 : indexBase);
		indices.push_back(indexBase + i);
	}

	std::vector<XMFLOAT2> textureCoordinates(positions.size());
	std::vector<XMFLOAT3> tangents(positions.size());

	Mesh mesh;
	mesh.LoadAttribute("POSITION", &positions[0], positions.size());
	mesh.LoadAttribute("NORMAL", &normals[0], normals.size());
	mesh.LoadAttribute("TANGENT", &tangents[0], tangents.size());
	mesh.LoadAttribute("TEX_COORD", &textureCoordinates[0], textureCoordinates.size());
	mesh.LoadIndexBuffer(indices);

	return new StaticMeshComponent(std::vector<Mesh>{mesh}, std::vector<Material>{material}, positions);
}