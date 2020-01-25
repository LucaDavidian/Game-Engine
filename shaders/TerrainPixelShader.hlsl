SamplerState textureSampler : register(s0);
Texture2D textureShadowDepthMap : register(t0);
Texture2D textureBlendMap : register(t1);
Texture2D texture1 : register(t2);
Texture2D texture2 : register(t3);
Texture2D texture3 : register(t4);
Texture2D texture4 : register(t5);

/**** lights ****/
#define DIRECTIONAL_LIGHT 0
#define POINT_LIGHT       1
#define SPOT_LIGHT        2

struct Light
{
	bool enabled;
	uint type;
	float3 color;
	float3 worldDirection;
	float3 worldPosition;
	float range;
	float intensity;
	float spotLightAngle;
};

#define NUM_LIGHTS 10

cbuffer Lights : register(b0)
{
	Light lights[NUM_LIGHTS];
};

/**** camera position ****/
cbuffer Camera : register(b1)
{
	float3 cameraWorldPosition;
	float shadowDistance;
};

float3 calculateLighting(Light light, float3 worldNormal, float3 worldPosition, float3 cameraWorldPosition);
float3 calculateDirectionalLight(Light light, float3 worldNormal, float3 toEyeVector);
float3 calculatePointLight(Light light, float3 worldPosition, float3 worldNormal, float3 toEyeVector);
float3 calculateSpotLight(Light light, float3 worldPosition, float3 worldNormal, float3 toEyeVector);
float3 diffuseLight(float3 lightColor, float3 toLightVector, float3 worldNormal);
float calculateAttenuation(float lightRange, float distance);
float calculateFalloff(float spotAngle, float3 fromLightVector, float3 spotDirection);

struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 worldPosition : WORLD_POSITION;
	float4 lightClipPosition : LIGHT_CLIP_POSITION;
	float3 worldNormal : WORLD_NORMAL;
	float2 textureCoordinates : TEX_COORD;
};

/**** pixel shader ****/
[earlydepthstencil]
float4 main(PixelShaderInput input) : SV_TARGET
{
	float texture1Intensity = textureBlendMap.Sample(textureSampler, input.textureCoordinates).r;
	float texture2Intensity = textureBlendMap.Sample(textureSampler, input.textureCoordinates).g;
	float texture3Intensity = textureBlendMap.Sample(textureSampler, input.textureCoordinates).b;

	float3 texture1Color = texture1.Sample(textureSampler, input.textureCoordinates * 5.0).rgb;
	float3 texture2Color = texture2.Sample(textureSampler, input.textureCoordinates * 15.0).rgb;
	float3 texture3Color = texture3.Sample(textureSampler, input.textureCoordinates * 15.0).rgb;
	float3 texture4Color = texture4.Sample(textureSampler, input.textureCoordinates * 30.0).rgb;

	float3 terrainColor = texture4Color * (1.0 - (texture1Intensity + texture2Intensity + texture3Intensity))
					  	+ texture1Color * texture1Intensity
						+ texture2Color * texture2Intensity
						+ texture3Color * texture3Intensity;

	/**** lighting calculations ****/
	float3 result = (float3)0;
	for (int i = 0; i < NUM_LIGHTS; i++)
		if (lights[i].enabled)
			if (lights[i].type != DIRECTIONAL_LIGHT && length(lights[i].worldPosition - input.worldPosition.xyz) > lights[i].range)
				;
			else
				result += calculateLighting(lights[i], normalize(input.worldNormal), input.worldPosition.xyz, cameraWorldPosition);

	terrainColor *= result;

	/**** directional shadow mapping ****/
	float2 shadowTexCoords;
	shadowTexCoords.x = (input.lightClipPosition.x / input.lightClipPosition.w + 1.0 ) / 2.0;
	shadowTexCoords.y = (-input.lightClipPosition.y / input.lightClipPosition.w + 1.0) / 2.0;

	float closestDepth = textureShadowDepthMap.Sample(textureSampler, shadowTexCoords).r;
	float fragmentDepth = input.lightClipPosition.z / input.lightClipPosition.w;

	// shadow transition
	float fragmentDistanceFromCamera = length(cameraWorldPosition - input.worldPosition.xyz); 
	float transitionLength = 10.0;
	float t = (fragmentDistanceFromCamera - (shadowDistance - transitionLength)) / transitionLength;
	float transitionFactor = clamp(t, 0.0, 1.0);

	// calculate shadows
	if (fragmentDepth >= closestDepth)
		terrainColor -= 0.15 * (1 - transitionFactor);

	return float4(terrainColor, 1.0);
}

/**** lighting calculations ****/
float3 calculateLighting(Light light, float3 worldNormal, float3 worldPosition, float3 cameraWorldPosition)
{
	float3 toEyeVector = normalize(cameraWorldPosition - worldPosition);

	switch (light.type)
	{
	case DIRECTIONAL_LIGHT:
		return calculateDirectionalLight(light, worldNormal, toEyeVector);
	case POINT_LIGHT:
		return calculatePointLight(light, worldPosition, worldNormal, toEyeVector);
	case SPOT_LIGHT:
		return calculateSpotLight(light, worldPosition, worldNormal, toEyeVector);
	default:
		return calculateSpotLight(light, worldPosition, worldNormal, toEyeVector); // todo: remove
		break;
	}
}

/**** directional lights ****/
float3 calculateDirectionalLight(Light light, float3 worldNormal, float3 toEyeVector)
{
	float3 toLightVector = normalize(-light.worldDirection);

	return diffuseLight(light.color, toLightVector, worldNormal);
}

/**** point lights ****/
float3 calculatePointLight(Light light, float3 worldPosition, float3 worldNormal, float3 toEyeVector)
{
	float3 toLightVector = normalize(light.worldPosition - worldPosition);

	float3 diffuse = diffuseLight(light.color, toLightVector, worldNormal);

	float attenuation = calculateAttenuation(light.range, length(worldPosition - light.worldPosition));

	return diffuse * attenuation * light.intensity;
}

/**** spot lights ****/
float3 calculateSpotLight(Light light, float3 worldPosition, float3 worldNormal, float3 toEyeVector)
{
	float3 toLightVector = normalize(light.worldPosition - worldPosition);
	float3 spotDirection = normalize(light.worldDirection);

	float3 diffuse = diffuseLight(light.color, toLightVector, worldNormal);

	float attenuation = calculateAttenuation(light.range, length(worldPosition - light.worldPosition));

	float falloff = calculateFalloff(light.spotLightAngle, -toLightVector, spotDirection);

	return diffuse = diffuse * attenuation * falloff * light.intensity;
}

/**** diffuse light ****/
float3 diffuseLight(float3 lightColor, float3 toLightVector, float3 worldNormal)
{
	float diffusionFactor = max(0.0, dot(toLightVector, worldNormal));
	return lightColor * diffusionFactor;
}

/**** light attenuation ****/
#define SMOOTHNESS 0.75

float calculateAttenuation(float lightRange, float distance)
{
	return 1.0 - smoothstep(lightRange * SMOOTHNESS, lightRange, distance);
}

/**** light falloff ****/
#define FALLOFF 0.5

float calculateFalloff(float spotAngle, float3 fromLightVector, float3 spotDirection)
{
	float directionAngleCos = 1.0 - dot(fromLightVector, spotDirection);
	float spotAngleCos = 1.0 - cos(radians(spotAngle / 2.0));
	return 1 - smoothstep(spotAngleCos * FALLOFF, spotAngleCos, directionAngleCos);
}