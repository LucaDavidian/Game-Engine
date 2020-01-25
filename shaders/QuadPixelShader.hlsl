Texture2D quadTexture : register(t0);
SamplerState textureSampler : register(s0);

cbuffer Material
{
	bool hasTexture;
	float3 color;
	float opacity;
};

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float2 textureCoordinates : TEX_COORD;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	if (!hasTexture)
		return float4(color, opacity);
	else
		return quadTexture.Sample(textureSampler, input.textureCoordinates) * float4(color, opacity);
}