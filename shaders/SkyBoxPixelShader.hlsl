TextureCube cubeMap : register(t0);
SamplerState textureSampler : register(s0);

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float3 textureCoordinates : TEX_COORD;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	return cubeMap.Sample(textureSampler, input.textureCoordinates);
}