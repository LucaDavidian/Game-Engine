cbuffer LightViewProjection
{
	float4x4 worldMatrix;
	float4x4 lightViewProjectionMatrix;
};

float4 main(float3 position : POSITION) : SV_Position
{
	float4 worldPosition = mul(worldMatrix, float4(position, 1.0));
	return mul(lightViewProjectionMatrix, worldPosition);
}