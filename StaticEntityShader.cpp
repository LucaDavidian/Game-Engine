#include "StaticEntityShader.h"
#include "Entity.h"
#include "PositionComponent.h"
#include "LightComponent.h"
#include "Material.h"
#include "GraphicsSystem.h"
#include "Error.h"

StaticEntityShader::StaticEntityShader() : Shader(L"shaders/StaticEntityVertexShader.hlsl", nullptr, L"shaders/StaticEntityPixelShader.hlsl")
{
	CreateInputLayout();
	CreateConstantBuffers();
	CreateSamplerStates();
}

void StaticEntityShader::CreateInputLayout()
{
	HRESULT hr;

	// describe input layout
	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEX_COORD", 0, DXGI_FORMAT_R32G32_FLOAT, 3, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// create input layout
	hr = GraphicsSystem::GetInstance().GetDevice()->CreateInputLayout(inputLayout, 4, mVertexShaderCode->GetBufferPointer(), mVertexShaderCode->GetBufferSize(), &mInputLayout);
	if (FAILED(hr))
		ErrorBox("input layout creation failed");

	mVertexShaderCode->Release();
}

void StaticEntityShader::CreateConstantBuffers()
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

	// create material constant buffer
	D3D11_BUFFER_DESC matBufDesc = {};
	matBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matBufDesc.ByteWidth = sizeof(MaterialConstantBuffer);
	matBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	matBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matBufDesc.MiscFlags = 0;
	matBufDesc.StructureByteStride = 0;

	hr = GraphicsSystem::GetInstance().GetDevice()->CreateBuffer(&matBufDesc, nullptr, &mMaterialConstantBuffer);  // empty buffer
	if (FAILED(hr))
		ErrorBox("constant buffer creation failed");

	// create lights constant buffer
	D3D11_BUFFER_DESC lightsBufDesc = {};
	lightsBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightsBufDesc.ByteWidth = sizeof(LightConstantBuffer);
	lightsBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightsBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	lightsBufDesc.MiscFlags = 0;
	lightsBufDesc.StructureByteStride = 0;

	hr = GraphicsSystem::GetInstance().GetDevice()->CreateBuffer(&lightsBufDesc, nullptr, &mLightConstantBuffer);  // empty buffer
	if (FAILED(hr))
		ErrorBox("constant buffer creation failed");

	// create camera constant buffer
	D3D11_BUFFER_DESC cameraBufDesc = {};
	cameraBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufDesc.ByteWidth = sizeof(CameraConstantBuffer);
	cameraBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cameraBufDesc.MiscFlags = 0;
	cameraBufDesc.StructureByteStride = 0;

	hr = GraphicsSystem::GetInstance().GetDevice()->CreateBuffer(&cameraBufDesc, nullptr, &mCameraConstantBuffer);  // empty buffer
	if (FAILED(hr))
		ErrorBox("constant buffer creation failed");
}

void StaticEntityShader::CreateSamplerStates()
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
	samplerDesc.MinLOD = 0.0f;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// create sampler state
	hr = GraphicsSystem::GetInstance().GetDevice()->CreateSamplerState(&samplerDesc, &mSamplerState);
	if (FAILED(hr))
		ErrorBox("sampler state creation failed");
}

void StaticEntityShader::UpdateTransformConstantBuffer(const XMFLOAT4X4 &worldMatrix, const XMFLOAT4X4 &worldInverseTransposeMatrix, const XMFLOAT4X4 &worldViewProjectionMatrix, const XMFLOAT4X4 &lightViewProjectionMatrix, const XMFLOAT4X4 &lightViewProjectionMatrixSpot)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	GraphicsSystem::GetInstance().GetDeviceContext()->Map(mTransformConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	TransformConstantBuffer *data(static_cast<TransformConstantBuffer*>(mappedResource.pData));
	data->worldMatrix = worldMatrix;
	data->worldInverseTransposeMatrix = worldInverseTransposeMatrix;
	data->worldViewProjectionMatrix = worldViewProjectionMatrix;
	data->lightViewProjectionMatrix = lightViewProjectionMatrix;
	data->lightViewProjectionMatrixSpot = lightViewProjectionMatrixSpot;

	GraphicsSystem::GetInstance().GetDeviceContext()->Unmap(mTransformConstantBuffer, 0);
}

void StaticEntityShader::UpdateLightConstantBuffer(const Vector<Entity*> &lights)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	GraphicsSystem::GetInstance().GetDeviceContext()->Map(mLightConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	LightConstantBuffer *data(static_cast<LightConstantBuffer*>(mappedResource.pData));
	int i;
	for (i = 0; i < lights.Size() && i < max_lights; i++)
	{
		PositionComponent *positionComponent = lights[i]->GetComponent<PositionComponent>();
		LightComponent *lightComponent = lights[i]->GetComponent<LightComponent>();

		data->lights[i].mEnabled = lightComponent->IsEnabled();
		data->lights[i].mType = static_cast<uint32_t>(lightComponent->GetType());
		data->lights[i].mColor = lightComponent->GetColor();
		data->lights[i].mDirection = positionComponent->GetAxisZ();
		data->lights[i].mPosition = positionComponent->GetPosition();
		data->lights[i].mRange = lightComponent->GetRange();
		data->lights[i].mIntensity = lightComponent->GetIntensity();
		data->lights[i].mSpotLightAngle = lightComponent->GetSpotLightAngle();
	}

	if (i < max_lights - 1)
		while (i < max_lights - 1)
			data->lights[i++].mEnabled = 0;

	GraphicsSystem::GetInstance().GetDeviceContext()->Unmap(mLightConstantBuffer, 0);
}

void StaticEntityShader::UpdateMaterialConstantBuffer(Material const &material)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	GraphicsSystem::GetInstance().GetDeviceContext()->Map(mMaterialConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	MaterialConstantBuffer *data(static_cast<MaterialConstantBuffer*>(mappedResource.pData));
	data->material.hasDiffuseMap = material.HasDiffuseMap();
	data->material.hasSpecularMap = material.HasSpecularMap();
	data->material.hasNormalMap = material.HasNormalMap();
	data->material.diffuseColor = material.GetDiffuseColor();
	data->material.specularColor = material.GetSpecularColor();
	data->material.specularPower = material.GetSpecularPower();
	data->material.tilingH = material.GetHorizontalTiling();
	data->material.tilingV = material.GetVerticalTiling();

	GraphicsSystem::GetInstance().GetDeviceContext()->Unmap(mMaterialConstantBuffer, 0);
}

void StaticEntityShader::UpdateCameraConstantBuffer(XMFLOAT3 const &cameraWorldPosition, float shadowDistance)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	GraphicsSystem::GetInstance().GetDeviceContext()->Map(mCameraConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	CameraConstantBuffer *data(static_cast<CameraConstantBuffer*>(mappedResource.pData));
	data->cameraWorldPosition = cameraWorldPosition;
	data->shadowDistance = shadowDistance;

	GraphicsSystem::GetInstance().GetDeviceContext()->Unmap(mCameraConstantBuffer, 0);
}

void StaticEntityShader::Use()
{
	// common shader set up
	Shader::Use();

	// set primitive topology
	GraphicsSystem::GetInstance().GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set constant buffers
	// vertex shader
	GraphicsSystem::GetInstance().GetDeviceContext()->VSSetConstantBuffers(0, 1, &mTransformConstantBuffer);
	// pixel shader
	GraphicsSystem::GetInstance().GetDeviceContext()->PSSetConstantBuffers(0, 1, &mMaterialConstantBuffer);
	GraphicsSystem::GetInstance().GetDeviceContext()->PSSetConstantBuffers(1, 1, &mLightConstantBuffer);
	GraphicsSystem::GetInstance().GetDeviceContext()->PSSetConstantBuffers(2, 1, &mCameraConstantBuffer);

	// set sampler state
	GraphicsSystem::GetInstance().GetDeviceContext()->PSSetSamplers(0, 1, &mSamplerState);
}

