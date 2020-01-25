#pragma once

#include <vector>
#include <DirectXMath.h>
#include "Skeleton.h"
#include "Model.h"

using namespace DirectX;

class Mesh;
class Texture;

class SkeletalModel
{
public:
	SkeletalModel(std::vector<XMFLOAT3> const &vertices, const std::vector<Mesh*> &meshes, const Skeleton &skeleton, const std::vector<std::vector<Texture*>> &textures, Material material);
	~SkeletalModel();
	std::vector<Mesh*> &GetMeshes() { return mMeshes; }
	Skeleton &GetSkeleton() { return mSkeleton; }
	std::vector<std::vector<Texture*>> &GetTextures() { return mTextures; }
	Material GetMaterial() { return mMaterial; }
private:
	std::vector<Mesh*> mMeshes;
	Skeleton mSkeleton;
	std::vector<std::vector<Texture*>> mTextures;
	Material mMaterial;
};

