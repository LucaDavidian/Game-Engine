#ifndef GEOMETRY_GENERATOR_H
#define GEOMETRY_GENERATOR_H

#include <string>
#include <DirectXMath.h>

using namespace DirectX;

class StaticMeshComponent;
class Material;

class GeometryGenerator
{
public:
	static StaticMeshComponent *GenerateSphere(float radius, Material material);
	static StaticMeshComponent *GenerateBox(XMFLOAT3 halfsize, Material material);
	static StaticMeshComponent *GenerateCylinder(float bottomRadius, float topRadius, float height, Material material);
	static StaticMeshComponent *GenerateCone(float radius, float height, Material material);
	static StaticMeshComponent *GeneratePlane(float width, float depth, Material material);
	static StaticMeshComponent *GenerateSkybox(const std::string &cubeMap);
private:
	GeometryGenerator() = default;
};

#endif  // GEOMETRY_GENERATOR_H

