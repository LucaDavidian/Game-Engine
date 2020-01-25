cbuffer Transform : register(b0)
{
	float4x4 worldViewProjectionMatrix;
};

struct VertexShaderInput
{
	float3 position : POSITION;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float3 textureCoordinates : TEX_COORD;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.position = mul(worldViewProjectionMatrix, float4(input.position, 1.0));
	output.textureCoordinates = input.position;

	return output;
}