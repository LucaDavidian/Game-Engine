#ifndef TERRAIN_SHADER_H
#define TERRAIN_SHADER_H

#include <vector>
#include "Shader.h"

struct Light;

class TerrainShader : public Shader
{
public:
	TerrainShader();
	~TerrainShader();
	void Use() override;
	void UpdateTransformConstantBuffer(const XMFLOAT4X4 &worldMatrix, const XMFLOAT4X4 &worldViewProjectionMatrix, XMFLOAT4X4 const &lightViewProjectionMatrix);
	void UpdateLightConstantBuffer(const std::vector<Light*> &lights);
	void UpdateClipPlaneConstantBuffer(XMFLOAT4 const &clipPlane);
	void UpdateCameraConstantBuffer(XMFLOAT3 const &cameraWorldPosition, float shadowDistance);
private:
	struct TransformConstantBuffer
	{
		XMFLOAT4X4 worldMatrix;
		XMFLOAT4X4 worldViewProjectionMatrix;
		XMFLOAT4X4 lightViewProjectionMatrix;
	};
	static const int max_lights = 10;
	struct LightConstantBuffer
	{
		struct
		{
			uint32_t mEnabled;
			uint32_t mType;
			float _padding0[2];
			XMFLOAT3 mColor;
			float _padding1;
			XMFLOAT3 mDirection;
			float _padding2;
			XMFLOAT3 mPosition;
			float mRange;
			float mIntensity;
			float mSpotLightAngle;
			float _padding3[2];
		} lights[max_lights];
	};
	struct CliPlaneConstantBuffer
	{
		XMFLOAT4 clipPlane;
	};
	struct CameraConstantBuffer
	{
		XMFLOAT3 cameraWorldPosition;
		float shadowDistance;
	};
	void CreateInputLayout() override;
	void CreateConstantBuffers();
	void CreateSamplerStates();
	ID3D11Buffer *mTransformConstantBuffer;
	ID3D11Buffer *mLightConstantBuffer;
	ID3D11Buffer *mClipPlaneConstantBuffer;
	ID3D11Buffer *mCameraConstantBuffer;
	ID3D11SamplerState *mSamplerState;
};

#endif  // TERRAIN_SHADER_H