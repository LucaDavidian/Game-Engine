#include "ModelLoader.h"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "StaticMeshComponent.h"
#include "Skeleton.h"

ModelLoader &ModelLoader::GetInstance()
{
	static ModelLoader instance;

	return instance;
}

StaticMeshComponent *ModelLoader::LoadStaticModel(const std::string &filePath)
{
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);

	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		ErrorBox(importer.GetErrorString());

	mDirectory = filePath.substr(0, filePath.find_last_of('/'));

	mModelMeshes.clear();
	mModelMaterials.clear();
	mModelVertexPositions.clear();

	ProcessNode(scene->mRootNode, scene);

	return new StaticMeshComponent(mModelMeshes, mModelMaterials, mModelVertexPositions);
}

void ModelLoader::ProcessNode(aiNode *node, const aiScene *scene)
{
	for (int i = 0; i < node->mNumMeshes; i++)
		ProcessMesh(scene->mMeshes[node->mMeshes[i]], scene);

	for (int i = 0; i < node->mNumChildren; i++)
		ProcessNode(node->mChildren[i], scene);
}

void ModelLoader::ProcessMesh(aiMesh *mesh, const aiScene *scene)
{
	// load index buffer
	std::vector<unsigned int> indices;

	for (int i = 0; i < mesh->mNumFaces; i++)
	{
		for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
			indices.push_back(mesh->mFaces[i].mIndices[j]);
	}

	// load positions
	std::vector<XMFLOAT3> positions;

	for (int i = 0; i < mesh->mNumVertices; i++)
	{
		positions.push_back(XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
		mModelVertexPositions.push_back(positions[i]);
	}

	// load normals
	std::vector<XMFLOAT3> normals;

	if (mesh->HasNormals())
		for (int i = 0; i < mesh->mNumVertices; i++)
			normals.push_back(XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
	else  // if model has no normal, procedurally calculate them
	{
		std::vector<std::vector<XMFLOAT3>> accumulatedNormals(mesh->mNumVertices);

		// calculate normal for each triangle and add them to each vertex in the triangle
		for (int i = 0; i < indices.size(); i += 3)
		{
			XMFLOAT3 p0 = positions[indices[i]];
			XMFLOAT3 p1 = positions[indices[i + 1]];
			XMFLOAT3 p2 = positions[indices[i + 2]];

			XMVECTOR v1 = XMLoadFloat3(&p1) - XMLoadFloat3(&p0);
			XMVECTOR v2 = XMLoadFloat3(&p2) - XMLoadFloat3(&p0);

			XMFLOAT3 triangleNormal;
			XMStoreFloat3(&triangleNormal, XMVector3Normalize(XMVector3Cross(v1, v2)));

			accumulatedNormals[indices[i]].push_back(triangleNormal);
			accumulatedNormals[indices[i + 1]].push_back(triangleNormal);
			accumulatedNormals[indices[i + 2]].push_back(triangleNormal);
		}

		// average vertex normals
		for (std::vector<XMFLOAT3> vertexNormals : accumulatedNormals)
		{
			XMVECTOR sum = XMVectorZero();

			for (XMFLOAT3 vertexNormal : vertexNormals)
				sum += XMLoadFloat3(&vertexNormal);

			XMFLOAT3 averagedNormal;
			XMStoreFloat3(&averagedNormal, sum / XMVector3Length(sum));

			normals.push_back(averagedNormal);
		}
	}

	//load texture coordinates
	std::vector<XMFLOAT2> textureCoordinates;

	if (mesh->HasTextureCoords(0))
		for (int i = 0; i < mesh->mNumVertices; i++)
			textureCoordinates.push_back(XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
	else
		for (int i = 0; i < mesh->mNumVertices; i++)
			textureCoordinates.push_back(XMFLOAT2(0.0f, 0.0f));

	// load tangents
	std::vector<XMFLOAT3> tangents;

	if (mesh->HasTangentsAndBitangents())
		for (int i = 0; i < mesh->mNumVertices; i++)
			tangents.push_back(XMFLOAT3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z));
	else  // if model has no tangents, procedurally calculate them
	{
		std::vector<std::vector<XMFLOAT3>> accumulatedTangents(mesh->mNumVertices);

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
	}

	// load vertex attributes and index buffer
	Mesh modelMesh;

	modelMesh.LoadAttribute("POSITION", &positions[0], positions.size());
	modelMesh.LoadAttribute("NORMAL", &normals[0], normals.size());
	modelMesh.LoadAttribute("TANGENT", &tangents[0], tangents.size());
	modelMesh.LoadAttribute("TEX_COORD", &textureCoordinates[0], textureCoordinates.size());
	modelMesh.LoadIndexBuffer(indices);

	mModelMeshes.push_back(modelMesh);

	// load materials
	Material modelMaterial;

	if (mesh->mMaterialIndex >= 0)
	{
		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

		std::vector<Texture> diffuseMaps = LoadMaterials(material, aiTextureType_DIFFUSE);
		modelMaterial.AddDiffuseMaps(diffuseMaps);

		std::vector<Texture> specularMaps = LoadMaterials(material, aiTextureType_SPECULAR);
		modelMaterial.AddSpecularMaps(specularMaps);
		
		std::vector<Texture> normalMaps = LoadMaterials(material, aiTextureType_NORMALS);
		modelMaterial.AddNormalMaps(normalMaps);

		mModelMaterials.push_back(modelMaterial);
	}
}

std::vector<Texture> ModelLoader::LoadMaterials(aiMaterial *material, aiTextureType aiType)
{
	std::vector<Texture> textures;

	for (int i = 0; i < material->GetTextureCount(aiType); i++)
	{
		aiString s;
		material->GetTexture(aiType, i, &s);
		std::string path(mDirectory + "/");
		path.append(s.C_Str());

		Texture texture = TextureManager::GetInstance().GetTexture(path);
		textures.push_back(texture);
	}

	return textures;
}

//SkeletalMeshComponent *ModelLoader::LoadSkeletalModel(const std::string &filePath)
//{
//	if (mSkeleton)
//		delete mSkeleton;
//
//	mSkeleton = new Skeleton;
//	mSkeleton->Load(filePath);
//
//	Assimp::Importer importer;
//	const aiScene *scene = importer.ReadFile(filePath, aiProcess_Triangulate | aiProcess_FlipUVs);
//
//	if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
//		ErrorBox(importer.GetErrorString());
//
//	mDirectory = filePath.substr(0, filePath.find_last_of('/'));
//
//	mModelVertexPositions.clear();
//	mModelMeshes.clear();
//	mModelMaterials.clear();
//
//	ProcessSkeletalNode(scene->mRootNode, scene);
//
//	return new SkeletalMeshComponent(mModelVertexPositions, mModelMeshes, *mSkeleton, mModelTextures, mModelMaterial);
//}

//void ModelLoader::ProcessSkeletalNode(aiNode *node, const aiScene *scene)
//{
//	for (int i = 0; i < node->mNumMeshes; i++)
//		ProcessSkeletalMesh(scene->mMeshes[node->mMeshes[i]], scene);
//
//	for (int i = 0; i < node->mNumChildren; i++)
//		ProcessSkeletalNode(node->mChildren[i], scene);
//}
//
//void ModelLoader::ProcessSkeletalMesh(aiMesh *mesh, const aiScene *scene)
//{
//	// load index buffer
//	std::vector<unsigned int> indices;
//
//	for (int i = 0; i < mesh->mNumFaces; i++)
//	{
//		for (int j = 0; j < mesh->mFaces[i].mNumIndices; j++)
//			indices.push_back(mesh->mFaces[i].mIndices[j]);
//	}
//
//	// load positions
//	for (int i = 0; i < mesh->mNumVertices; i++)
//		mModelVertexPositions.push_back(XMFLOAT3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z));
//
//	// load normals
//	std::vector<XMFLOAT3> normals;
//
//	if (mesh->HasNormals())
//		for (int i = 0; i < mesh->mNumVertices; i++)
//			normals.push_back(XMFLOAT3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z));
//	else  // if model has no normal, procedurally calculate them
//	{
//		std::vector<std::vector<XMFLOAT3>> accumulatedNormals(mesh->mNumVertices);
//
//		// calculate normal for each triangle and add them to each vertex in the triangle
//		for (int i = 0; i < indices.size(); i += 3)
//		{
//			XMFLOAT3 p0 = mModelVertexPositions[indices[i]];
//			XMFLOAT3 p1 = mModelVertexPositions[indices[i + 1]];
//			XMFLOAT3 p2 = mModelVertexPositions[indices[i + 2]];
//
//			XMVECTOR v1 = XMLoadFloat3(&p1) - XMLoadFloat3(&p0);
//			XMVECTOR v2 = XMLoadFloat3(&p2) - XMLoadFloat3(&p0);
//
//			XMFLOAT3 triangleNormal;
//			XMStoreFloat3(&triangleNormal, XMVector3Normalize(XMVector3Cross(v1, v2)));
//
//			accumulatedNormals[indices[i]].push_back(triangleNormal);
//			accumulatedNormals[indices[i + 1]].push_back(triangleNormal);
//			accumulatedNormals[indices[i + 2]].push_back(triangleNormal);
//		}
//
//		// average vertex normals
//		for (std::vector<XMFLOAT3> vertexNormals : accumulatedNormals)
//		{
//			XMVECTOR sum = XMVectorZero();
//
//			for (XMFLOAT3 vertexNormal : vertexNormals)
//				sum += XMLoadFloat3(&vertexNormal);
//
//			XMFLOAT3 averagedNormal;
//			XMStoreFloat3(&averagedNormal, sum / XMVector3Length(sum));
//
//			normals.push_back(averagedNormal);
//		}
//	}
//
//	//load texture coordinates
//	std::vector<XMFLOAT2> textureCoordinates;
//
//	if (mesh->HasTextureCoords(0))
//		for (int i = 0; i < mesh->mNumVertices; i++)
//			textureCoordinates.push_back(XMFLOAT2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y));
//	else
//		for (int i = 0; i < mesh->mNumVertices; i++)
//			textureCoordinates.push_back(XMFLOAT2(0.0f, 0.0f));
//
//	// load bone ids and weights
//	std::vector<std::vector<std::pair<unsigned int, float>>> boneIDWeights(mesh->mNumVertices);
//
//	for (int i = 0; i < mesh->mNumBones; i++)
//	{
//		aiBone *bone = mesh->mBones[i];
//		unsigned int boneID = mSkeleton->GetBoneIndex(bone->mName.C_Str());
//		for (int i = 0; i < bone->mNumWeights; i++)
//		{
//			aiVertexWeight weight = bone->mWeights[i];
//			boneIDWeights[weight.mVertexId].push_back(std::make_pair(boneID, weight.mWeight));
//		}
//	}
//
//	// pick only 4 most influencing bones per vertex 
//	std::vector<XMINT4> boneIDs;
//	std::vector<XMFLOAT4> boneWeights;
//
//	for (int i = 0; i < mesh->mNumVertices; i++)
//	{
//		std::vector<std::pair<unsigned int, float>> vertexBoneData = boneIDWeights[i];
//
//		if (vertexBoneData.size() > 4)
//		{
//			// take the 4 most influential weights and normalize
//			std::sort(vertexBoneData.begin(), vertexBoneData.end(), [](const std::pair<int, float> &p1, std::pair<int, float> const &p2) -> bool { return p1.second > p2.second; });  // sort by weight (descending order)
//
//			float sum = 0.0f;
//			for (int i = 0; i < 4; i++)
//				sum += vertexBoneData[i].second;
//			for (int i = 0; i < 4; i++)
//				vertexBoneData[i].second /= sum;
//
//			boneIDs.emplace_back(vertexBoneData[0].first, vertexBoneData[1].first, vertexBoneData[2].first, vertexBoneData[3].first);
//			boneWeights.emplace_back(vertexBoneData[0].second, vertexBoneData[1].second, vertexBoneData[2].second, vertexBoneData[3].second);
//		}
//		else if (vertexBoneData.size() <= 4)
//		{
//			int i = 0;
//			float ids[4];
//			float weights[4];
//			while (i < vertexBoneData.size())
//			{
//				ids[i] = vertexBoneData[i].first;
//				weights[i] = vertexBoneData[i].second;
//				i++;
//			}
//			// fill extra bone ids and weights with zeroes
//			while (i < 4)
//			{
//				ids[i] = 0;
//				weights[i] = 0.0f;
//				i++;
//			}
//
//			boneIDs.emplace_back(ids[0], ids[1], ids[2], ids[3]);
//			boneWeights.emplace_back(weights[0], weights[1], weights[2], weights[3]);
//		}
//	}
//
//	// load tangents
//	std::vector<XMFLOAT3> tangents;
//
//	if (mesh->HasTangentsAndBitangents())
//		for (int i = 0; i < mesh->mNumVertices; i++)
//			tangents.push_back(XMFLOAT3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z));
//	else  // if model has no tangents, procedurally calculate them
//	{
//		std::vector<std::vector<XMFLOAT3>> accumulatedTangents(mesh->mNumVertices);
//
//		for (int i = 0; i < indices.size(); i += 3)
//		{
//			XMFLOAT3 p0 = mModelVertexPositions[indices[i]];
//			XMFLOAT3 p1 = mModelVertexPositions[indices[i + 1]];
//			XMFLOAT3 p2 = mModelVertexPositions[indices[i + 2]];
//
//			XMVECTOR v1 = XMLoadFloat3(&p1) - XMLoadFloat3(&p0);
//			XMVECTOR v2 = XMLoadFloat3(&p2) - XMLoadFloat3(&p0);
//
//			float du1 = textureCoordinates[indices[i + 1]].x - textureCoordinates[indices[i]].x;
//			float dv1 = textureCoordinates[indices[i + 1]].y - textureCoordinates[indices[i]].y;
//			float du2 = textureCoordinates[indices[i + 2]].x - textureCoordinates[indices[i]].x;
//			float dv2 = textureCoordinates[indices[i + 2]].y - textureCoordinates[indices[i]].y;
//
//			float det = du1 * dv2 - dv1 * du2;
//
//			XMFLOAT3 triangleTangent;
//			XMStoreFloat3(&triangleTangent, (dv2 * v1 - dv1 * v2) / det);
//
//			accumulatedTangents[indices[i]].push_back(triangleTangent);
//			accumulatedTangents[indices[i + 1]].push_back(triangleTangent);
//			accumulatedTangents[indices[i + 2]].push_back(triangleTangent);
//		}
//
//		// average vertex tangents
//		for (std::vector<XMFLOAT3> vertexTangents : accumulatedTangents)
//		{
//			XMVECTOR sum = XMVectorZero();
//
//			for (XMFLOAT3 vertexTangent : vertexTangents)
//				sum += XMLoadFloat3(&vertexTangent);
//
//			XMFLOAT3 averagedTangent;
//			XMStoreFloat3(&averagedTangent, sum / XMVector3Length(sum));
//
//			tangents.push_back(averagedTangent);
//		}
//	}
//
//	// load vertex attributes and index buffer
//	Mesh modelMesh;
//
//	modelMesh.LoadAttribute("POSITION", &mModelVertexPositions[0], mModelVertexPositions.size());
//	modelMesh.LoadAttribute("NORMAL", &normals[0], normals.size());
//	modelMesh.LoadAttribute("TANGENT", &tangents[0], tangents.size());
//	modelMesh.LoadAttribute("TEX_COORD", &textureCoordinates[0], textureCoordinates.size());
//	modelMesh.LoadAttribute("BONE_IDS", &boneIDs[0], boneIDs.size());
//	modelMesh.LoadAttribute("BONE_WEIGHTS", &boneWeights[0], boneWeights.size());
//	modelMesh.LoadIndexBuffer(indices);
//
//	mModelMeshes.push_back(modelMesh);
//
//	// load materials
//	Material modelMaterial;
//
//	if (mesh->mMaterialIndex >= 0)
//	{
//		aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];
//
//		std::vector<Texture> diffuseMaps = LoadMaterials(material, aiTextureType_DIFFUSE);
//		modelMaterial.AddDiffuseMaps(diffuseMaps);
//
//		std::vector<Texture> specularMaps = LoadMaterials(material, aiTextureType_SPECULAR);
//		modelMaterial.AddSpecularMaps(diffuseMaps);
//
//		std::vector<Texture> normalMaps = LoadMaterials(material, aiTextureType_NORMALS);
//		modelMaterial.AddNormalMaps(diffuseMaps);
//
//		mModelMaterials.push_back(modelMaterial);
//	}
//}