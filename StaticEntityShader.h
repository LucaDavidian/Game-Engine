#ifndef STATIC_ENTITY_SHADER_H
#define STATIC_ENTITY_SHADER_H

#include "data structures/Vector.h"
#include "Shader.h"
#include <cstdint>

class Entity;
class Material;

class StaticEntityShader : public Shader
{
public:
	StaticEntityShader();

	void Use() override;

	void UpdateTransformConstantBuffer(const XMFLOAT4X4 &worldMatrix, const XMFLOAT4X4 &worldInverseTransposeMatrix, const XMFLOAT4X4 &worldViewProjectionMatrix, const XMFLOAT4X4 &lightViewProjectionMatrix, const XMFLOAT4X4 &lightViewProjectionMatrixSpot);
	void UpdateLightConstantBuffer(Vector<Entity*> const &lights);
	void UpdateMaterialConstantBuffer(Material const &material);
	void UpdateCameraConstantBuffer(const XMFLOAT3 &cameraWorldPosition, float shadowDistance);
private:
	struct TransformConstantBuffer
	{
		XMFLOAT4X4 worldMatrix;
		XMFLOAT4X4 worldInverseTransposeMatrix;
		XMFLOAT4X4 worldViewProjectionMatrix;
		XMFLOAT4X4 lightViewProjectionMatrix;
		XMFLOAT4X4 lightViewProjectionMatrixSpot;
	};
	struct MaterialConstantBuffer
	{
		struct
		{
			uint32_t hasDiffuseMap;
			uint32_t hasSpecularMap;
			uint32_t hasNormalMap;
			float _padding0;
			XMFLOAT3 diffuseColor;
			float _padding1;
			XMFLOAT3 specularColor;
			float specularPower;
			float tilingH;
			float tilingV;
			float _padding2[2];
		} material;
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
	struct CameraConstantBuffer
	{
		XMFLOAT3 cameraWorldPosition;
		float shadowDistance;
	};

	void CreateInputLayout() override;
	void CreateConstantBuffers();
	void CreateSamplerStates();

	ID3D11SamplerState *mSamplerState;
	ID3D11Buffer *mTransformConstantBuffer;
	ID3D11Buffer *mMaterialConstantBuffer;
	ID3D11Buffer *mLightConstantBuffer;
	ID3D11Buffer *mCameraConstantBuffer;
};

#endif  // STATIC_ENTITY_SHADER_H

