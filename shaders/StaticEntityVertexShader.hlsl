cbuffer Transform : register(b0)
{
	float4x4 worldMatrix;                     // matrices are stored in column-major order by default (use row_major to store in row-major order)
	float4x4 worldInverseTransposeMatrix;     // matrix data is passed in row-major order (so by default matrices are transposed)
	float4x4 worldViewProjectionMatrix;
	float4x4 lightViewProjectionMatrix;
	float4x4 lightViewProjectionMatrixSpot;
};

struct VertexShaderInput
{
	float3 position : POSITION;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float2 textureCoordinates : TEX_COORD;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float4 worldPosition : WORLD_POSITION;
	float4 clipPosition : CLIP_POSITION;
	float4 lightClipPosition : LIGHT_CLIP_POSITION;
	float4 lightClipPositionSpot : LIGHT_CLIP_POSITION_SPOT;
	float3 worldNormal : WORLD_NORMAL;
	float3 worldTangent : WORLD_TANGENT;
	float2 textureCoordinates : TEX_COORD;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.position = mul(worldViewProjectionMatrix, float4(input.position, 1.0));   
	output.worldPosition = mul(worldMatrix, float4(input.position, 1.0));
	output.clipPosition = output.position;

	output.lightClipPosition = mul(lightViewProjectionMatrix, output.worldPosition);
	output.lightClipPositionSpot = mul(lightViewProjectionMatrixSpot, output.worldPosition);

	output.worldNormal = mul(worldInverseTransposeMatrix, float4(input.normal, 0.0));
	output.worldTangent = mul(worldMatrix, float4(input.tangent, 0.0));
	output.textureCoordinates = input.textureCoordinates;

	return output;
}