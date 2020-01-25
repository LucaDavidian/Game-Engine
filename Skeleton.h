#pragma once

#include <string>
#include <vector>
#include <map>
//#include "Bone.h"
#include "assimp/scene.h"
//#include "Box.h"

class Skeleton
{
public:
	Skeleton();
	Skeleton(const Skeleton &rhs);
	~Skeleton();
	void Load(std::string const &modelFilePath);
	void SetBoneLocalTransformMatrices(std::vector<XMFLOAT4X4> const &boneLocalTransformMatrices);
	std::vector<XMFLOAT4X4> GetBoneTransformMatrices();
	unsigned int GetBoneIndex(std::string const &boneName) const;
	//std::vector<Bone> &GetBones() { return mBones; }
	//Box &GetCollisionBox();
private:	
	XMFLOAT4X4 ConvertMatrix(aiMatrix4x4 const &matrix);
	void ProcessMesh(aiMesh *mesh);
	//void AddBone(const Bone &bone);
	void BuildSkeleton(aiNode *node);
	void Update();
	//std::vector<Bone> mBones;                            // bone index corresponds to bone ID in vertex attribute
	std::map<std::string, unsigned int> mBoneMap;        // mapping from bone name to bone index 
	unsigned int mRootBoneIndex;                         // root bone's index
	//Box mCollisionBox;
};

