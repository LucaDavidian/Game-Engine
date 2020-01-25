#include "SkyBoxShader.h"
#include "GraphicsSystem.h"
#include "Error.h"

SkyBoxShader::SkyBoxShader() : Shader(L"shaders/SkyBoxVertexShader.hlsl", nullptr, L"shaders/SkyBoxPixelShader.hlsl")
{
	CreateInputLayout();
	CreateConstantBuffers();
	CreateSamplerStates();
}

void SkyBoxShader::CreateInputLayout()
{
	HRESULT hr;

	// describe input layout
	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// create input layout
	hr = GraphicsSystem::GetInstance().GetDevice()->CreateInputLayout(inputLayout, 1, mVertexShaderCode->GetBufferPointer(), mVertexShaderCode->GetBufferSize(), &mInputLayout);
	if (FAILED(hr))
		ErrorBox("input layout creation failed");

	mVertexShaderCode->Release();
}

void SkyBoxShader::CreateConstantBuffers()
{
	HRESULT hr;

	// create transform constant buffer
	D3D11_BUFFER_DESC transformBufDesc = {};
	transformBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	transformBufDesc.ByteWidth = sizeof(TransformConstantBuffer);
	transformBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	transformBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	transformBufDesc.MiscFlags = 0;
	transformBufDesc.StructureByteStride = 0;

	hr = GraphicsSystem::GetInstance().GetDevice()->CreateBuffer(&transformBufDesc, nullptr, &mTransformConstantBuffer);  // empty buffer
	if (FAILED(hr))
		ErrorBox("constant buffer creation failed");
}

void SkyBoxShader::CreateSamplerStates()
{
	HRESULT hr;

	// describe sampler state
	D3D11_SAMPLER_DESC samplerDesc = {};
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// create sampler state
	hr = GraphicsSystem::GetInstance().GetDevice()->CreateSamplerState(&samplerDesc, &mSamplerState);
	if (FAILED(hr))
		ErrorBox("sampler state creation failed");
}

void SkyBoxShader::UpdateTransformConstantBuffer(XMFLOAT4X4 worldViewProjectionMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	GraphicsSystem::GetInstance().GetDeviceContext()->Map(mTransformConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	TransformConstantBuffer *data(static_cast<TransformConstantBuffer*>(mappedResource.pData));
	data->worldViewProjectionMatrix = worldViewProjectionMatrix;

	GraphicsSystem::GetInstance().GetDeviceContext()->Unmap(mTransformConstantBuffer, 0);
}

void SkyBoxShader::Use()
{
	// common shader set up
	Shader::Use();

	// set primitive topology
	GraphicsSystem::GetInstance().GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set constant buffers
	// vertex shader
	GraphicsSystem::GetInstance().GetDeviceContext()->VSSetConstantBuffers(0, 1, &mTransformConstantBuffer);

	// set sampler state
	GraphicsSystem::GetInstance().GetDeviceContext()->PSSetSamplers(0, 1, &mSamplerState);
}