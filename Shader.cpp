#include "Shader.h"
#include "Error.h"
#include "GraphicsSystem.h"
#include "GraphicsSystem.h"
#include <d3dcompiler.h>

Shader::Shader(const wchar_t *vertexShaderFilePath, const wchar_t *geometryShaderFilePath, const wchar_t *pixelShaderFilePath)
{
	CompileShaders(vertexShaderFilePath, geometryShaderFilePath, pixelShaderFilePath);
}

Shader::~Shader()
{
	// TODO:: release resources
}

void Shader::CompileShaders(const wchar_t *vertexShaderFilePath, const wchar_t *geometryShaderFilePath, const wchar_t *pixelShaderFilePath)
{
	HRESULT hr;
	ID3DBlob *geometryShaderCode;
	ID3DBlob *pixelShaderCode;

	// compile shaders
	ID3DBlob *error;

	if (vertexShaderFilePath)
	{
		hr = D3DCompileFromFile(vertexShaderFilePath, nullptr, nullptr, "main", "vs_5_0", 0, 0, &mVertexShaderCode, &error);
		if (FAILED(hr))
		{
			OutputDebugStringA((LPCSTR)error->GetBufferPointer());
			ErrorBox("vertex shader compilation failed");
		}

		hr = GraphicsSystem::GetInstance().GetDevice()->CreateVertexShader(mVertexShaderCode->GetBufferPointer(), mVertexShaderCode->GetBufferSize(), nullptr, &mVertexShader);
		if (FAILED(hr))
			ErrorBox("vertex shader creation failed");
	}

	if (geometryShaderFilePath)
	{
		hr = D3DCompileFromFile(geometryShaderFilePath, nullptr, nullptr, "main", "gs_5_0", 0, 0, &geometryShaderCode, &error);
		if (FAILED(hr))
		{
			OutputDebugStringA((LPCSTR)error->GetBufferPointer());
			ErrorBox("geometry shader compilation failed");
		}

		hr = GraphicsSystem::GetInstance().GetDevice()->CreateGeometryShader(geometryShaderCode->GetBufferPointer(), geometryShaderCode->GetBufferSize(), nullptr, &mGeometryShader);
		if (FAILED(hr))
			ErrorBox("geometry shader creation failed");

		geometryShaderCode->Release();
	}

	if (pixelShaderFilePath)
	{
		hr = D3DCompileFromFile(pixelShaderFilePath, nullptr, nullptr, "main", "ps_5_0", 0, 0, &pixelShaderCode, &error);
		if (FAILED(hr))
		{
			OutputDebugStringA((LPCSTR)error->GetBufferPointer());
			ErrorBox("pixel shader compilation failed");
		}

		hr = GraphicsSystem::GetInstance().GetDevice()->CreatePixelShader(pixelShaderCode->GetBufferPointer(), pixelShaderCode->GetBufferSize(), nullptr, &mPixelShader);
		if (FAILED(hr))
			ErrorBox("pixel shader creation failed");

		pixelShaderCode->Release();
	}
}

void Shader::Use()
{
	// set shaders
	GraphicsSystem::GetInstance().GetDeviceContext()->VSSetShader(mVertexShader, nullptr, 0);

	if (mGeometryShader)
		GraphicsSystem::GetInstance().GetDeviceContext()->GSSetShader(mGeometryShader, nullptr, 0);
	else
		GraphicsSystem::GetInstance().GetDeviceContext()->GSSetShader(nullptr, nullptr, 0);

	GraphicsSystem::GetInstance().GetDeviceContext()->PSSetShader(mPixelShader, nullptr, 0);

	// set input layout
	GraphicsSystem::GetInstance().GetDeviceContext()->IASetInputLayout(mInputLayout);
}
