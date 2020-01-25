#pragma once

#include "Shader.h"

class StaticShadowShader : public Shader
{
public:
	StaticShadowShader();
	~StaticShadowShader();
	void UpdateTransformConstantBuffer(XMFLOAT4X4 worldMatrix, XMFLOAT4X4 lightViewProjectionMatrix);
	void Use() override;
private:
	struct TransformConstantBuffer
	{
		XMFLOAT4X4 worldMatrix;
		XMFLOAT4X4 lightViewProjectionMatrix;
	};
	void CreateInputLayout() override;
	void CreateConstantBuffers();
	ID3D11Buffer *mTransformConstantBuffer;
};

