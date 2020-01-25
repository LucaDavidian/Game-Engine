#include "SkeletalModel.h"

SkeletalModel::SkeletalModel(std::vector<XMFLOAT3> const &vertices, const std::vector<Mesh*> &meshes, const Skeleton &skeleton, const std::vector<std::vector<Texture*>> &textures, Material material)
	: mMeshes(meshes), mSkeleton(skeleton), mTextures(textures), mMaterial(material)
{
}

SkeletalModel::~SkeletalModel()
{
}
