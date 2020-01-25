#ifndef TEXTURE_H
#define TEXTURE_H

#include <d3d11.h>
#include <string>

class Texture
{
public:
	Texture() : mShaderResourceView(nullptr) {}
	Texture(std::string const &textureFilePath);
	Texture(ID3D11ShaderResourceView *resourceView) : mShaderResourceView(resourceView) {}
	Texture(unsigned width, unsigned height, DXGI_FORMAT format);

	void Update(void *data, unsigned xOffset, unsigned yOffset, unsigned width, unsigned height);  

	void SetResourceView(ID3D11ShaderResourceView *shaderResourceView) { mShaderResourceView = shaderResourceView; }

	void Bind(unsigned int slot) const;
	void Unbind() const;
private:
	void LoadDDS(const std::wstring &textureFilePath);
	void LoadWIC(const std::wstring &textureFilePath);
	
	ID3D11ShaderResourceView *mShaderResourceView;
	mutable int mSlot = -1;
};

#endif  // TEXTURE_H

