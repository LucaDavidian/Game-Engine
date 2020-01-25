#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class Shader
{
public:
	~Shader();
	virtual void Use() = 0;
protected:
	Shader(const wchar_t *vertexShaderFilePath, const wchar_t *geometryShaderFilePath, const wchar_t *pixelShaderFilePath);
	ID3DBlob * mVertexShaderCode;
	ID3D11InputLayout * mInputLayout;
private:
	void CompileShaders(const wchar_t *vertexShaderFilePath, const wchar_t *geometryShaderFilePath, const wchar_t *pixelShaderFilePath);
	virtual void CreateInputLayout() = 0;
	ID3D11VertexShader *mVertexShader;
	ID3D11GeometryShader *mGeometryShader;
	ID3D11PixelShader *mPixelShader;
};

