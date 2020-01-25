#pragma once

#include "Shader.h"

class TextShader : public Shader
{
public:
	TextShader();
	~TextShader();
	void UpdateTransformConstantBuffer(const XMFLOAT4X4 &worldMatrix);
	void UpdateTextParamsConstantBuffer(const XMFLOAT3 &textColor);
	void Use() override;
private:
	struct TransformConstantBuffer
	{
		XMFLOAT4X4 worldMatrix;
	};
	struct TextParamsConstantBuffer
	{
		XMFLOAT3 textColor;
		float _padding;
	};
	void CreateInputLayout() override;
	void CreateConstantBuffers();
	void CreateSamplerStates();
	ID3D11Buffer *mTransformConstantBuffer;
	ID3D11Buffer *mTextParamsConstantBuffer;
	ID3D11SamplerState *mSamplerState;
};

