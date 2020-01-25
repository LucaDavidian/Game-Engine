cbuffer Transform : register(b0)
{
	float4x4 worldMatrix;
	float4x4 worldViewProjectionMatrix;
	float4x4 lightViewProjectionMatrix;
};

cbuffer ClipPlane : register(b1)
{
	float4 clipPlane;
};

struct VertexShaderInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float2 textureCoordinates : TEX_COORD;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float4 worldPosition : WORLD_POSITION;
	float4 lightClipPosition : LIGHT_CLIP_POSITION;
	float3 worldNormal : WORLD_NORMAL;
	float2 textureCoordinates : TEX_COORD;
	float clipDistance0 : SV_CLIPDISTANCE;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.position = mul(worldViewProjectionMatrix, float4(input.position, 1.0));
	output.worldPosition = mul(worldMatrix, float4(input.position, 1.0));
	output.lightClipPosition = mul(lightViewProjectionMatrix, output.worldPosition);
	output.worldNormal = mul(worldMatrix, float4(input.normal, 0.0)).xyz;
	output.textureCoordinates = input.textureCoordinates;

	output.clipDistance0 = dot(output.worldPosition, clipPlane);

	return output;
}