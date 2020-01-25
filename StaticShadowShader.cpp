#include "StaticShadowShader.h"
#include "GraphicsSystem.h"
#include "Error.h"

StaticShadowShader::StaticShadowShader() : Shader(L"shaders/StaticShadowVertexShader.hlsl", nullptr, nullptr)
{
	CreateInputLayout();
	CreateConstantBuffers();
}

StaticShadowShader::~StaticShadowShader()
{
}

void StaticShadowShader::CreateInputLayout()
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

void StaticShadowShader::CreateConstantBuffers()
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

void StaticShadowShader::UpdateTransformConstantBuffer(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 lightViewProjectionMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	GraphicsSystem::GetInstance().GetDeviceContext()->Map(mTransformConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	TransformConstantBuffer *data(static_cast<TransformConstantBuffer*>(mappedResource.pData));
	data->worldMatrix = worldMatrix;
	data->lightViewProjectionMatrix = lightViewProjectionMatrix;
	GraphicsSystem::GetInstance().GetDeviceContext()->Unmap(mTransformConstantBuffer, 0);
}

void StaticShadowShader::Use()
{
	// common shader set up
	Shader::Use();

	// set primitive topology
	GraphicsSystem::GetInstance().GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set constant buffers
	// vertex shader
	GraphicsSystem::GetInstance().GetDeviceContext()->VSSetConstantBuffers(0, 1, &mTransformConstantBuffer);
}


