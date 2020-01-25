cbuffer Transform : register(b0)
{
	float4x4 worldMatrix;
};
struct VertexShaderInput
{
	float2 position : POSITION;
	float2 textureCoordinates : TEX_COORD;
};

struct VertexShaderOutput
{
	float4 position : SV_POSITION;
	float2 textureCoordinates : TEX_COORD;
};

VertexShaderOutput main(VertexShaderInput input)
{
	VertexShaderOutput output;

	output.position = mul(worldMatrix, float4(input.position, 0.0, 1.0));
	output.textureCoordinates = input.textureCoordinates;
	
	return output;
}