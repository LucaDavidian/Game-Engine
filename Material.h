
#ifndef MATERIAL_H
#define MATERIAL_H

#include "Texture.h"
#include "TextureManager.h"
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;

class Material
{
public:
	void AddDiffuseMap(std::string const &diffuseMapName) { Texture diffuseMap = TextureManager::GetInstance().GetTexture(diffuseMapName); mDiffuseMaps.push_back(diffuseMap); }
	void AddDiffuseMaps(const std::vector<Texture> &diffuseMaps) { for (Texture texture : diffuseMaps) mDiffuseMaps.push_back(texture); }
	void AddSpecularMap(const std::string &specularMapName) { Texture specularMap = TextureManager::GetInstance().GetTexture(specularMapName);  mSpecularMaps.push_back(specularMap); }
	void AddSpecularMaps(const std::vector<Texture> &diffuseMaps) { for (Texture texture : diffuseMaps) mSpecularMaps.push_back(texture); }
	void AddNormalMap(const std::string &normalMapName) { Texture normalMap = TextureManager::GetInstance().GetTexture(normalMapName); mNormalMaps.push_back(normalMap); }
	void AddNormalMaps(const std::vector<Texture> &diffuseMaps) { for (Texture texture : diffuseMaps) mNormalMaps.push_back(texture); }

	void SetDiffuseColor(const XMFLOAT3 &diffuseColor) { mDiffuseColor = diffuseColor; }
	void SetSpecularColor(const XMFLOAT3 &specularColor) { mSpecularColor = specularColor; }
	void SetSpecularPower(float specularPower) { mSpecularPower = specularPower; }

	void SetTiling(float horizontal, float vertical) { mHorizontalTiling = horizontal; mVerticalTiling = vertical; }

	const std::vector<Texture> GetDiffuseMaps() const { return mDiffuseMaps; }
	const std::vector<Texture> GetSpecularMaps() const { return mSpecularMaps; }
	const std::vector<Texture> GetNormalMaps() const { return mNormalMaps; }

	XMFLOAT3 GetDiffuseColor() const { return mDiffuseColor; }
	XMFLOAT3 GetSpecularColor() const { return mSpecularColor; }
	float GetSpecularPower() const { return mSpecularPower; }

	float GetHorizontalTiling() const { return mHorizontalTiling; }
	float GetVerticalTiling() const { return mVerticalTiling; }

	bool HasDiffuseMap() const { return mDiffuseMaps.size(); }
	bool HasSpecularMap() const { return mSpecularMaps.size(); }
	bool HasNormalMap() const { return mNormalMaps.size(); }
private:
	std::vector<Texture> mDiffuseMaps;
	std::vector<Texture> mSpecularMaps;
	std::vector<Texture> mNormalMaps;

	XMFLOAT3 mDiffuseColor = XMFLOAT3(1.0f, 1.0f, 1.0f);
	XMFLOAT3 mSpecularColor = XMFLOAT3();
	float mSpecularPower = 16.0f;

	float mHorizontalTiling = 1.0f;
	float mVerticalTiling = 1.0f;
};

#endif  // MATERIAL_H