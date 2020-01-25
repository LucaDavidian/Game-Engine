#include "TerrainShader.h"
#include "Light.h"
#include "Graphics.h"


TerrainShader::TerrainShader() : Shader(L"shaders/TerrainVertexShader.hlsl", nullptr, L"shaders/TerrainPixelShader.hlsl")
{
	CreateInputLayout();
	CreateConstantBuffers();
	CreateSamplerStates();
}

TerrainShader::~TerrainShader()
{
}

void TerrainShader::CreateInputLayout()
{
	HRESULT hr;

	// describe input layout
	D3D11_INPUT_ELEMENT_DESC inputLayout[] =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEX_COORD", 0, DXGI_FORMAT_R32G32_FLOAT, 2, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};

	// create input layout
	hr = Graphics::GetInstance().GetDevice()->CreateInputLayout(inputLayout, 3, mVertexShaderCode->GetBufferPointer(), mVertexShaderCode->GetBufferSize(), &mInputLayout);
	if (FAILED(hr))
		ErrorBox(L"input layout creation failed");

	mVertexShaderCode->Release();
}

void TerrainShader::CreateConstantBuffers()
{
	HRESULT hr;

	D3D11_BUFFER_DESC transformBufDesc = {};
	transformBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	transformBufDesc.ByteWidth = sizeof(TransformConstantBuffer);
	transformBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	transformBufDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	transformBufDesc.MiscFlags = 0;
	transformBufDesc.StructureByteStride = 0;

	hr = Graphics::GetInstance().GetDevice()->CreateBuffer(&transformBufDesc, nullptr, &mTransformConstantBuffer);  // empty buffer
	if (FAILED(hr))
		ErrorBox(L"constant buffer creation failed");

	D3D11_BUFFER_DESC lightsBufDesc = {};
	lightsBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	lightsBufDesc.ByteWidth = sizeof(LightConstantBuffer);
	lightsBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	lightsBufDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	lightsBufDesc.MiscFlags = 0;
	lightsBufDesc.StructureByteStride = 0;

	hr = Graphics::GetInstance().GetDevice()->CreateBuffer(&lightsBufDesc, nullptr, &mLightConstantBuffer);  // empty buffer
	if (FAILED(hr))
		ErrorBox(L"constant buffer creation failed");

	D3D11_BUFFER_DESC clipBufDesc = {};
	clipBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	clipBufDesc.ByteWidth = sizeof(CliPlaneConstantBuffer);
	clipBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	clipBufDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	clipBufDesc.MiscFlags = 0;
	clipBufDesc.StructureByteStride = 0;

	hr = Graphics::GetInstance().GetDevice()->CreateBuffer(&clipBufDesc, nullptr, &mClipPlaneConstantBuffer);  // empty buffer
	if (FAILED(hr))
		ErrorBox(L"constant buffer creation failed");

	D3D11_BUFFER_DESC cameraBufDesc = {};
	cameraBufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cameraBufDesc.ByteWidth = sizeof(CameraConstantBuffer);
	cameraBufDesc.Usage = D3D11_USAGE_DYNAMIC;
	cameraBufDesc.CPUAccessFlags = D3D10_CPU_ACCESS_WRITE;
	cameraBufDesc.MiscFlags = 0;
	cameraBufDesc.StructureByteStride = 0;

	hr = Graphics::GetInstance().GetDevice()->CreateBuffer(&cameraBufDesc, nullptr, &mCameraConstantBuffer);  // empty buffer
	if (FAILED(hr))
		ErrorBox(L"constant buffer creation failed");
}

void TerrainShader::CreateSamplerStates()
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
	hr = Graphics::GetInstance().GetDevice()->CreateSamplerState(&samplerDesc, &mSamplerState);
	if (FAILED(hr))
		ErrorBox(L"sampler state creation failed");
}

void TerrainShader::UpdateTransformConstantBuffer(const XMFLOAT4X4 &worldMatrix, const XMFLOAT4X4 &worldViewProjectionMatrix, XMFLOAT4X4 const &lightViewProjectionMatrix)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Graphics::GetInstance().GetDeviceContext()->Map(mTransformConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	TransformConstantBuffer *data(static_cast<TransformConstantBuffer*>(mappedResource.pData));
	data->worldMatrix = worldMatrix;
	data->worldViewProjectionMatrix = worldViewProjectionMatrix;
	data->lightViewProjectionMatrix = lightViewProjectionMatrix;
	Graphics::GetInstance().GetDeviceContext()->Unmap(mTransformConstantBuffer, 0);
}

void TerrainShader::UpdateLightConstantBuffer(const std::vector<Light*> &lights)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Graphics::GetInstance().GetDeviceContext()->Map(mLightConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	LightConstantBuffer *data(static_cast<LightConstantBuffer*>(mappedResource.pData));
	for (int i = 0; i < lights.size() && i < max_lights; i++)
	{
		data->lights[i].mEnabled = lights[i]->mEnabled;
		data->lights[i].mType = lights[i]->mType;
		data->lights[i].mColor = lights[i]->mColor;
		data->lights[i].mDirection = lights[i]->mDirection;
		data->lights[i].mPosition = lights[i]->mPosition;
		data->lights[i].mRange = lights[i]->mRange;
		data->lights[i].mIntensity = lights[i]->mIntensity;
		data->lights[i].mSpotLightAngle = lights[i]->mSpotLightAngle;
	}
	Graphics::GetInstance().GetDeviceContext()->Unmap(mLightConstantBuffer, 0);
}

void TerrainShader::UpdateClipPlaneConstantBuffer(XMFLOAT4 const &clipPlane)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Graphics::GetInstance().GetDeviceContext()->Map(mClipPlaneConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CliPlaneConstantBuffer *data(static_cast<CliPlaneConstantBuffer*>(mappedResource.pData));
	data->clipPlane = clipPlane;
	Graphics::GetInstance().GetDeviceContext()->Unmap(mClipPlaneConstantBuffer, 0);
}

void TerrainShader::UpdateCameraConstantBuffer(XMFLOAT3 const &cameraWorldPosition, float shadowDistance)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	Graphics::GetInstance().GetDeviceContext()->Map(mCameraConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CameraConstantBuffer *data(static_cast<CameraConstantBuffer*>(mappedResource.pData));
	data->cameraWorldPosition = cameraWorldPosition;
	data->shadowDistance = shadowDistance;
	Graphics::GetInstance().GetDeviceContext()->Unmap(mCameraConstantBuffer, 0);
}

void TerrainShader::Use()
{
	// common shader set up
	Shader::Use();

	// set primitive topology
	Graphics::GetInstance().GetDeviceContext()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// set constant buffers
	// vertex shader
	Graphics::GetInstance().GetDeviceContext()->VSSetConstantBuffers(0, 1, &mTransformConstantBuffer);
	Graphics::GetInstance().GetDeviceContext()->VSSetConstantBuffers(1, 1, &mClipPlaneConstantBuffer);
	// pixel shader
	Graphics::GetInstance().GetDeviceContext()->PSSetConstantBuffers(0, 1, &mLightConstantBuffer);
	Graphics::GetInstance().GetDeviceContext()->PSSetConstantBuffers(1, 1, &mCameraConstantBuffer);

	// set sampler state
	Graphics::GetInstance().GetDeviceContext()->PSSetSamplers(0, 1, &mSamplerState);
}