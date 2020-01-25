#ifndef STATIC_MESH_COMPONENT_H
#define STATIC_MESH_COMPONENT_H

#include <vector>
#include "Component.h"
#include "Mesh.h"
#include "Material.h"

class StaticMeshComponent : public Component
{
public:
	StaticMeshComponent(const std::vector<Mesh> &meshes, const std::vector<Material> &materials, const std::vector<XMFLOAT3> &vertices) : mMeshes(meshes), mMaterials(materials), mVertices(vertices) {}

	const std::vector<Mesh> &GetMeshes() const { return mMeshes; }
	const std::vector<Material> &GetMaterials() const { return mMaterials; }
	const std::vector<XMFLOAT3> &GetVertices() const { return mVertices; }
private:
	std::vector<XMFLOAT3> mVertices;
	std::vector<Mesh> mMeshes;
	std::vector<Material> mMaterials;
};

#endif  // STATIC_MESH_COMPONENT_H