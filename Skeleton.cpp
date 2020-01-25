#include "Skeleton.h"
#include "assimp/Importer.hpp"
#include "Error.h"

Skeleton::Skeleton()
{
}

Skeleton::~Skeleton()
{
}

Skeleton::Skeleton(const Skeleton &rhs) : mBoneMap(rhs.mBoneMap), mBones(rhs.mBones), mRootBoneIndex(rhs.mRootBoneIndex)
{
	// reconnect bones' children
	for (Bone &bone : mBones)
		for (Bone *&childBone : bone.mChildren)
			childBone = &mBones[mBoneMap[childBone->GetName()]];

	//mBoundingBoxMesh = rhs.mBoundingBoxMesh;
}

void Skeleton::Load(std::string const &modelFilePath)
{
	Assimp::Importer importer;
	aiScene const *scene = importer.ReadFile(modelFilePath, 0);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		ErrorBox(L"cannot load skeleton");

	for (int i = 0; i < scene->mNumMeshes; i++)
		ProcessMesh(scene->mMeshes[i]);

	// build bone tree hierarchy
	BuildSkeleton(scene->mRootNode);

	// set model in T-pose (bind/rest pose)
	Update();
}

XMFLOAT4X4 Skeleton::ConvertMatrix(aiMatrix4x4 const &matrix)
{
	XMFLOAT4X4 result;
	XMStoreFloat4x4(&result, XMMatrixSet(matrix.a1, matrix.b1, matrix.c1, matrix.d1,
		matrix.a2, matrix.b2, matrix.c2, matrix.d2,
		matrix.a3, matrix.b3, matrix.c3, matrix.d3,
		matrix.a4, matrix.b4, matrix.c4, matrix.d4));
	return result;
}

void Skeleton::ProcessMesh(aiMesh *mesh)
{
	for (int i = 0; i < mesh->mNumBones; i++)
	{
		aiBone *bone = mesh->mBones[i];
		Bone modelBone(bone->mName.C_Str());
		modelBone.SetOffsetMatrix(ConvertMatrix(bone->mOffsetMatrix));
		AddBone(modelBone);
	}
}

void Skeleton::AddBone(const Bone &bone)
{
	static unsigned int boneIndex = 0;

	if (mBoneMap.find(bone.GetName()) == mBoneMap.end())
	{
		//mBoneMap.insert(std::pair<std::string, unsigned int>(bone.GetName(), boneIndex++));
		mBoneMap[bone.GetName()] = boneIndex++;
		mBones.push_back(bone);
	}
}  

void Skeleton::BuildSkeleton(aiNode *node)
{
	static bool found;

	// serch tree for bone node
	std::map<std::string, unsigned int>::iterator it = mBoneMap.find(node->mName.C_Str());
	if (it != mBoneMap.end())
	{
		if (!found)  // first bone encountered is the root bone
		{
			mRootBoneIndex = it->second;
			found = true;
		}

		// store local bind transform matrix
		mBones[it->second].SetLocalBindTransformMatrix(ConvertMatrix(node->mTransformation));
		
		// set T-pose (bind/rest pose)as default
		mBones[it->second].SetLocalTransformMatrix(ConvertMatrix(node->mTransformation));

		// add children
		for (int i = 0; i < node->mNumChildren; i++)
			if (mBoneMap.find(node->mChildren[i]->mName.C_Str()) != mBoneMap.end())
				mBones[it->second].AddChild(&mBones[mBoneMap[node->mChildren[i]->mName.C_Str()]]);
	}

	// recursively search all node tree
	for (int i = 0; i < node->mNumChildren; i++)
		BuildSkeleton(node->mChildren[i]);
}

void Skeleton::SetBoneLocalTransformMatrices(std::vector<XMFLOAT4X4> const &boneLocalTransformMatrices)
{
	int i = 0;
	for (Bone &bone : mBones)
		bone.SetLocalTransformMatrix(boneLocalTransformMatrices[i++]); 

	// update skeleton's bone transform matrices
	Update();
}

std::vector<XMFLOAT4X4> Skeleton::GetBoneTransformMatrices()
{
	std::vector<XMFLOAT4X4> boneTransformMatrices;
	for (Bone const &bone : mBones)
		boneTransformMatrices.push_back(bone.GetTransformMatrix());

	return boneTransformMatrices;
}

unsigned int Skeleton::GetBoneIndex(std::string const &boneName) const
{
	std::map<std::string, unsigned int>::const_iterator it = mBoneMap.find(boneName);
	if (it != mBoneMap.end())
		return it->second;
	else
		return -1;  // bone not present
}

void Skeleton::Update()
{
	// update bone transforms
	XMFLOAT4X4 identityMatrix;
	XMStoreFloat4x4(&identityMatrix, XMMatrixIdentity());
	mBones[mRootBoneIndex].CalculateTransformMatrix(identityMatrix);

	// update bounding box
	//float minX = 0.0f, maxX = 0.0f;
	//float minY = 100.0f, maxY = -100.0f;
	//float minZ = 0.0f, maxZ = 0.0f;

	//for (Bone bone : mBones)
	//{
	//	XMFLOAT3(*boundingBox)[8] = &bone.GetBox().GetVertices();
	//	XMMATRIX boneTranformMatrix = XMLoadFloat4x4(&bone.GetTransformMatrix());

	//	XMFLOAT3 box[8];

	//	XMStoreFloat3(&box[0], XMVector3TransformCoord(XMLoadFloat3(&(*boundingBox)[0]), boneTranformMatrix));
	//	XMStoreFloat3(&box[1], XMVector3TransformCoord(XMLoadFloat3(&(*boundingBox)[1]), boneTranformMatrix));
	//	XMStoreFloat3(&box[2], XMVector3TransformCoord(XMLoadFloat3(&(*boundingBox)[2]), boneTranformMatrix));
	//	XMStoreFloat3(&box[3], XMVector3TransformCoord(XMLoadFloat3(&(*boundingBox)[3]), boneTranformMatrix));
	//	XMStoreFloat3(&box[4], XMVector3TransformCoord(XMLoadFloat3(&(*boundingBox)[4]), boneTranformMatrix));
	//	XMStoreFloat3(&box[5], XMVector3TransformCoord(XMLoadFloat3(&(*boundingBox)[5]), boneTranformMatrix));
	//	XMStoreFloat3(&box[6], XMVector3TransformCoord(XMLoadFloat3(&(*boundingBox)[6]), boneTranformMatrix));
	//	XMStoreFloat3(&box[7], XMVector3TransformCoord(XMLoadFloat3(&(*boundingBox)[7]), boneTranformMatrix));

	//	for (int i = 0; i < 8; i++)
	//	{
	//		if (minX > box[i].x)
	//			minX = box[i].x;
	//		if (maxX < box[i].x)
	//			maxX = box[i].x;
	//		if (minY > box[i].y)
	//			minY = box[i].y;
	//		if (maxY < box[i].y)
	//			maxY = box[i].y;
	//		if (minZ > box[i].z)
	//			minZ = box[i].z;
	//		if (maxZ < box[i].z)
	//			maxZ = box[i].z;
	//	}
	//}

	//mCollisionBox.SetVertices({XMFLOAT3(minX, minY, minZ), XMFLOAT3(minX, minY, maxZ), XMFLOAT3(minX, maxY, maxZ), XMFLOAT3(minX, maxY, minZ), XMFLOAT3(maxX, minY, minZ), XMFLOAT3(maxX, minY, maxZ), XMFLOAT3(maxX, maxY, maxZ), XMFLOAT3(maxX, maxY, minZ)});
}
