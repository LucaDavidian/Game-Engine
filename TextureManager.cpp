
#include "TextureManager.h"

Texture &TextureManager::GetTexture(const std::string &textureFilePath)
{
	std::map<std::string, Texture>::iterator it = mLoadedTextures.find(textureFilePath);

	if (it == mLoadedTextures.end())
	{
		Texture texture(textureFilePath);
		mLoadedTextures.insert(std::pair<std::string, Texture>(textureFilePath, texture));
	}

	return mLoadedTextures.at(textureFilePath);
}
