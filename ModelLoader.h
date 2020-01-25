#pragma once

#include <string>
#include <vector>
#include <DirectXMath.h>
#include "assimp/scene.h"
#include "Texture.h"
#include "Material.h"

using namespace DirectX;

class StaticMeshComponent;
class SkeletalMeshComponent;
class Mesh;

class ModelLoader
{
public:
	static ModelLoader &GetInstance();
	StaticMeshComponent *LoadStaticModel(const std::string &filePath);
	SkeletalMeshComponent *LoadSkeletalModel(const std::string &filePath);
private:
	ModelLoader() = default;
	void ProcessNode(aiNode *node, const aiScene *scene);
	void ProcessSkeletalNode(aiNode *node, const aiScene *scene);
	void ProcessMesh(aiMesh *mesh, const aiScene *scene);
	void ProcessSkeletalMesh(aiMesh *mesh, const aiScene *scene);
	std::vector<Texture> LoadMaterials(aiMaterial *material, aiTextureType type);
	std::string mDirectory;
	std::vector<XMFLOAT3> mModelVertexPositions;
	std::vector<Mesh> mModelMeshes;
	std::vector<Material> mModelMaterials;
	//Skeleton *mSkeleton;
};

