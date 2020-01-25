#pragma once

#include "Shader.h"

class QuadShader : public Shader
{
public:
	QuadShader();
	~QuadShader();
	void UpdateTransformConstantBuffer(const XMFLOAT4X4 &worldMatrix);
	void UpdateMaterialConstantBuffer(bool hasTexture, const XMFLOAT3 &color, float opacity);
	void Use() override;
private:
	struct TransformConstantBuffer
	{
		XMFLOAT4X4 worldMatrix;
	};
	struct MaterialConstantBuffer
	{
		uint32_t hasTexture;
		XMFLOAT3 color;
		float opacity;
		XMFLOAT3 _padding;
	};
	void CreateInputLayout() override;
	void CreateConstantBuffers();
	void CreateSamplerStates();
	ID3D11Buffer *mTransformConstantBuffer;
	ID3D11Buffer *mMaterialConstantBuffer;
	ID3D11SamplerState *mSamplerState;
};

