#include "Texture.h"
#include "GraphicsSystem.h"
#include "DDSTextureLoader.h"
#include "WICTextureLoader.h"
#include "Error.h"

Texture::Texture(std::string const &textureFilePath) : mShaderResourceView(nullptr)
{
	// multibyte string to wide char (UTF-16) string 
	wchar_t wstrName[100];
	int wchars_num = MultiByteToWideChar(CP_UTF8, 0, textureFilePath.c_str(), -1, NULL, 0);
	MultiByteToWideChar(CP_UTF8, 0, textureFilePath.c_str(), -1, wstrName, wchars_num);

	std::wstring wideTexturefilePath(wstrName);
	std::wstring extension = wideTexturefilePath.substr(wideTexturefilePath.find_last_of('.'));

	if (extension == L".png" || extension == L".jpg" || extension == L".tga")
		LoadWIC(wideTexturefilePath);
	else if (extension == L".dds")
		LoadDDS(wideTexturefilePath);
	else
		ErrorBox("unsupported texture file format");
}

Texture::Texture(unsigned width, unsigned height, DXGI_FORMAT format)
{
	D3D11_TEXTURE2D_DESC textureDesc;

	textureDesc.Width = width;
	textureDesc.Height = height;
	textureDesc.Format = format;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	ID3D11Texture2D *texture;
	HRESULT hr = GraphicsSystem::GetInstance().GetDevice()->CreateTexture2D(&textureDesc, nullptr, &texture);  // empty texture resource
	
	if (FAILED(hr))
		ErrorBox("couldn't create texture resource");

	hr = GraphicsSystem::GetInstance().GetDevice()->CreateShaderResourceView(texture, nullptr, &mShaderResourceView);
	if (FAILED(hr))
		ErrorBox("couldn't create shader resource view from texture");

	texture->Release();
}

void Texture::LoadWIC(const std::wstring &textureFilePath)
{
	ID3D11Resource *resource;
	HRESULT hr = DirectX::CreateWICTextureFromFile(GraphicsSystem::GetInstance().GetDevice(), GraphicsSystem::GetInstance().GetDeviceContext(), textureFilePath.c_str(), &resource, &mShaderResourceView);

	if (FAILED(hr))
		ErrorBox("couldn't create shader resource view");

	resource->Release();  // resource view creation increases resource's ref-counting by 1
}

void Texture::LoadDDS(const std::wstring &textureFilePath)
{
	ID3D11Resource *resource;
	HRESULT hr = DirectX::CreateDDSTextureFromFile(GraphicsSystem::GetInstance().GetDevice(), GraphicsSystem::GetInstance().GetDeviceContext(), textureFilePath.c_str(), &resource, &mShaderResourceView);

	if (FAILED(hr))
		ErrorBox("couldn't create shader resource view");

	resource->Release();  // resource view creation increases resource's ref-counting by 1
}

void Texture::Update(void *data, unsigned xOffset, unsigned yOffset, unsigned width, unsigned height)
{
	ID3D11Resource *texture;
	mShaderResourceView->GetResource(&texture);

	D3D11_BOX box{};
	box.left = xOffset;
	box.right = xOffset + width;;
	box.top = yOffset;
	box.bottom = yOffset + height;
	box.front = 0;
	box.back = 1;

	GraphicsSystem::GetInstance().GetDeviceContext()->UpdateSubresource(texture, 0, &box, data, width, 0);

	texture->Release();
}

void Texture::Bind(unsigned int slot) const 
{
	mSlot = slot;
	GraphicsSystem::GetInstance().GetDeviceContext()->PSSetShaderResources(mSlot, 1, &mShaderResourceView);
}

void Texture::Unbind() const
{
	ID3D11ShaderResourceView *nullResource = nullptr;
	GraphicsSystem::GetInstance().GetDeviceContext()->PSSetShaderResources(mSlot, 1, &nullResource);
	mSlot = -1;
}
