Texture2D fontTextureAtlas : register(t0);
SamplerState textureSampler : register(s0);

cbuffer TextParameters : register(b0)
{
	float3 textColor;
};

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float2 textureCoordinates : TEX_COORD;
};

float4 main(PixelShaderInput input) : SV_TARGET
{
	float alpha = fontTextureAtlas.Sample(textureSampler, input.textureCoordinates).r;  
		
	return float4(textColor, alpha);
}