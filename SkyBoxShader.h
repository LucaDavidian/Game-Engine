#ifndef SKYBOX_SHADER_H
#define SKYBOX_SHADER_H

#include "Shader.h"

class SkyBoxShader : public Shader
{
public:
	SkyBoxShader();

	void Use() override;

	void UpdateTransformConstantBuffer(XMFLOAT4X4 worldViewProjectionMatrix);
private:
	struct TransformConstantBuffer
	{
		XMFLOAT4X4 worldViewProjectionMatrix;
	};

	void CreateInputLayout() override;
	void CreateConstantBuffers();
	void CreateSamplerStates();

	ID3D11Buffer *mTransformConstantBuffer;
	ID3D11SamplerState *mSamplerState;
};

#endif  // SKYBOX_SHADER_H

