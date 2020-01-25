SamplerState textureSampler : register(s0);
Texture2D shadowDepthMap : register(t0);
Texture2D diffuseMap : register(t1);
Texture2D specularMap : register(t2);
Texture2D normalMap : register(t3);
Texture2D shadowDepthMapSpot : register(t4);

/**** materials ****/
struct Material
{
	bool    hasDiffuseMap;
	bool    hasSpecularMap;
	bool    hasNormalMap;
	float3  diffuseColor;
	float3  specularColor;
	float   specularPower;
	float tilingH;
	float tilingV;
};

cbuffer Materials : register(b0)
{
	Material material;
};

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

cbuffer Lights : register(b1)
{
	Light lights[NUM_LIGHTS];
};

/**** camera position ****/
cbuffer Camera : register(b2)
{
	float3 cameraWorldPosition;
	float shadowDistance;
};

float3 doNormalMapping(float3 tangentSpaceNormal, float3 worldNormal, float3 worldTangent);
float3 doBumpMapping(Texture2D bumpMap, float2 textureCoordinates, float3 worldNormal, float3 worldTangent);

struct LightResult
{
	float3 diffuse;
	float3 specular;
};

LightResult calculateLighting(Light light, Material material, float3 worldNormal, float3 worldPosition, float3 cameraWorldPosition);
LightResult calculateDirectionalLight(Light light, float3 worldNormal, float3 toEyeVector, Material material);
LightResult calculatePointLight(Light light, float3 worldPosition, float3 worldNormal, float3 toEyeVector, Material material);
LightResult calculateSpotLight(Light light, float3 worldPosition, float3 worldNormal, float3 toEyeVector, Material material);
float3 diffuseLight(float3 lightColor, float3 toLightVector, float3 worldNormal);
float3 specularLight(float3 lightColor, float3 fromLightVector, float3 worldNormal, float3 toEyeVector, Material material);
float calculateAttenuation(float lightRange, float distance);
float calculateFalloff(float spotAngle, float3 fromLightVector, float3 spotDirection);

struct PixelShaderInput
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

/**** pixel shader ****/
[earlydepthstencil]
float4 main(PixelShaderInput input) : SV_TARGET
{
	/**** process materials properties and textures ****/

	// tiling
	input.textureCoordinates.x *= material.tilingH;
	input.textureCoordinates.y *= material.tilingV;

	/* diffuse material property */
	float3 diffuse = (float3)0;
	if (material.hasDiffuseMap)
		diffuse = diffuseMap.Sample(textureSampler, input.textureCoordinates).rgb;
	else
		diffuse = material.diffuseColor;

	/* ambient light */
	float3 ambient = diffuse * 0.1;

	/* specular material property */
	float3 specular = (float3)0;
	if (material.hasSpecularMap)
		specular = specularMap.Sample(textureSampler, input.textureCoordinates).rgb;
	else
		specular = material.specularColor;

	/* normal mapping */
	float3 normal = (float3)0;
	if (material.hasNormalMap)
	{
		float3 tangentSpaceNormal = normalMap.Sample(textureSampler, input.textureCoordinates).rgb  * 2.0 - 1.0;
		normal = doNormalMapping(tangentSpaceNormal, normalize(input.worldNormal), normalize(input.worldTangent));
	}
	else
		normal = normalize(input.worldNormal);

	/**** lighting calculations ****/
	LightResult lightResult = (LightResult)0;

	for (int i = 0; i < NUM_LIGHTS; i++)
		if (lights[i].enabled)
			if (lights[i].type != DIRECTIONAL_LIGHT && length(lights[i].worldPosition - input.worldPosition.xyz) > lights[i].range)
				;
			else if (lights[i].type == SPOT_LIGHT)
			{
				float2 shadowTexCoords;
				shadowTexCoords.x = (input.lightClipPositionSpot.x / input.lightClipPositionSpot.w + 1.0) / 2.0;
				shadowTexCoords.y = (-input.lightClipPositionSpot.y / input.lightClipPositionSpot.w + 1.0) / 2.0;

				float fragmentDepth = input.lightClipPositionSpot.z / input.lightClipPositionSpot.w;
				float closestDepth = shadowDepthMapSpot.Sample(textureSampler, shadowTexCoords).r;

				if (closestDepth + 0.0001 > fragmentDepth)
				{
					float3 worldNormal = normal; float3 worldPosition = input.worldPosition.xyz;
					float3 toEyeVector = normalize(cameraWorldPosition - worldPosition);

					float3 toLightVector = normalize(lights[i].worldPosition - worldPosition);
					float3 spotDirection = normalize(lights[i].worldDirection);

					/* diffuse reflection */
					float3 diffuse = diffuseLight(lights[i].color, toLightVector, worldNormal);

					/* specular reflection */
					float3 specular = specularLight(lights[i].color, -toLightVector, worldNormal, toEyeVector, material);

					/* attenuation */
					float attenuation = calculateAttenuation(lights[i].range, length(worldPosition - lights[i].worldPosition));

					/* falloff */
					float falloff = calculateFalloff(lights[i].spotLightAngle, -toLightVector, spotDirection);

					LightResult result;
					result.diffuse = diffuse * attenuation * falloff * lights[i].intensity;
					result.specular = specular * attenuation * falloff * lights[i].intensity;

					lightResult.diffuse += result.diffuse;
					lightResult.specular += result.specular;
				}	
			}
			else
			{
				LightResult result = calculateLighting(lights[i], material, normal, input.worldPosition.xyz, cameraWorldPosition);
				lightResult.diffuse += result.diffuse;
				lightResult.specular += result.specular;
			}	

	diffuse *= lightResult.diffuse;
	specular *= lightResult.specular;

	float3 totalColor = diffuse + specular + ambient;
	
	/**** directional shadow mapping ****/
	float2 shadowTexCoords;
	shadowTexCoords.x = (input.lightClipPosition.x / input.lightClipPosition.w + 1.0) / 2.0;
	shadowTexCoords.y = (-input.lightClipPosition.y / input.lightClipPosition.w + 1.0) / 2.0;

	float fragmentDepth = input.lightClipPosition.z / input.lightClipPosition.w;
	float closestDepth = shadowDepthMap.Sample(textureSampler, shadowTexCoords).r;
	
	// shadow transition
	float fragmentDistanceFromCamera = length(cameraWorldPosition - input.worldPosition.xyz);
	float transitionLength = 10.0;
	float t = (fragmentDistanceFromCamera - (shadowDistance - transitionLength)) / transitionLength;
	float transitionFactor = clamp(t, 0.0, 1.0);

	// calculate shadows
	if (fragmentDepth > closestDepth + 0.0005)
		totalColor -= 0.05 * (1 - transitionFactor);

	return float4(totalColor, 1.0);
}

/**** normal mapping calculations ****/
float3 doNormalMapping(float3 tangentSpaceNormal, float3 worldNormal, float3 worldTangent)
{
	worldTangent = normalize(worldTangent - dot(worldTangent, worldNormal) * worldNormal);
	float3 worldBitangent = normalize(cross(worldNormal, worldTangent));

	float3x3 TBN = float3x3(worldTangent, worldBitangent, worldNormal);  // matrix constructor takes row vectors

	return normalize(mul(tangentSpaceNormal, TBN));
}

/**** bump mapping calculations ****/
//float3 doBumpMapping(Texture2D bumpMap, float2 textureCoordinates, float3 worldNormal, float3 worldTangent)
//{
//	float2 offsetU;
//	float2 offsetV;
//
//	float bumpValue0 = bumpMap.Sample(textureSampler, textureCoordinates).r;
//	float bumpValue1 = bumpMap.Sample(textureSampler, textureCoordinates + offsetU).r;
//	float bumpValue2 = bumpMap.Sample(textureSampler, textureCoordinates + offsetV).r;
//
//	float3 dPdU = float3(textureCoordinates + offsetU, bumpValue1) - float3(textureCoordinates, bumpValue0);
//	float3 dPdV = float3(textureCoordinates + offsetV, bumpValue2) - float3(textureCoordinates, bumpValue0);
//
//	return normalize(cross(dPdU, dPdV));
//}

/**** lighting calculations ****/
LightResult calculateLighting(Light light, Material material, float3 worldNormal, float3 worldPosition, float3 cameraWorldPosition)
{
	float3 toEyeVector = normalize(cameraWorldPosition - worldPosition);

	switch (light.type)
	{
		case DIRECTIONAL_LIGHT:
			return calculateDirectionalLight(light, worldNormal, toEyeVector, material);
		case POINT_LIGHT:
			return calculatePointLight(light, worldPosition, worldNormal, toEyeVector, material);
		case SPOT_LIGHT:
			return calculateSpotLight(light, worldPosition, worldNormal, toEyeVector, material);
		default:
			LightResult unlit = { (float3)0, (float3)0 };
			return unlit; 
			break;
	}
}

/**** directional lights ****/
LightResult calculateDirectionalLight(Light light, float3 worldNormal, float3 toEyeVector, Material material)
{
	float3 toLightVector = normalize(-light.worldDirection);

	/* diffuse reflection */
	float3 diffuse = diffuseLight(light.color, toLightVector, worldNormal);

	/* specular reflection */
	float3 specular = specularLight(light.color, -toLightVector, worldNormal, toEyeVector, material);

	LightResult result;
	result.diffuse = diffuse * light.intensity;
	result.specular = specular * light.intensity;

	return result;
}

/**** point lights ****/
LightResult calculatePointLight(Light light, float3 worldPosition, float3 worldNormal, float3 toEyeVector, Material material)
{
	float3 toLightVector = normalize(light.worldPosition - worldPosition);

	/* diffuse reflection */
	float3 diffuse = diffuseLight(light.color, toLightVector, worldNormal);

	/* specular reflection */
	float3 specular = specularLight(light.color, -toLightVector, worldNormal, toEyeVector, material);

	/* attenuation */
	float attenuation = calculateAttenuation(light.range, length(worldPosition - light.worldPosition));

	LightResult result;
	result.diffuse = diffuse * attenuation * light.intensity;
	result.specular = specular * attenuation * light.intensity;
	
	return result;
}

/**** spot lights ****/
LightResult calculateSpotLight(Light light, float3 worldPosition, float3 worldNormal, float3 toEyeVector, Material material)
{
	float3 toLightVector = normalize(light.worldPosition - worldPosition);
	float3 spotDirection = normalize(light.worldDirection);

	/* diffuse reflection */
	float3 diffuse = diffuseLight(light.color, toLightVector, worldNormal);

	/* specular reflection */
	float3 specular = specularLight(light.color, -toLightVector, worldNormal, toEyeVector, material);

	/* attenuation */
	float attenuation = calculateAttenuation(light.range, length(worldPosition - light.worldPosition));

	/* falloff */
	float falloff = calculateFalloff(light.spotLightAngle, -toLightVector, spotDirection);

	LightResult result;
	result.diffuse = diffuse * attenuation * falloff * light.intensity;
	result.specular = specular * attenuation * falloff * light.intensity;

	return result;
}

/**** diffuse light ****/
float3 diffuseLight(float3 lightColor, float3 toLightVector, float3 worldNormal)
{
	float diffusionFactor = max(0.0, dot(toLightVector, worldNormal));
	return lightColor * diffusionFactor;
}

/**** specular light ****/
float3 specularLight(float3 lightColor, float3 fromLightVector, float3 worldNormal, float3 toEyeVector, Material material)
{
	float3 reflected = normalize(reflect(fromLightVector, worldNormal));

	float specularFactor = pow(max(0.0, dot(reflected, toEyeVector)), material.specularPower);

	return lightColor * specularFactor;
}

/**** light attenuation ****/
#define LINEAR 0.2
#define QUADRATIC 0.4

float calculateAttenuation(float lightRange, float distance)
{
	return 1.0 / (2.0 + LINEAR * distance + QUADRATIC * distance * distance);
}

/**** light falloff ****/
#define FALLOFF 0.1

float calculateFalloff(float spotAngle, float3 fromLightVector, float3 spotDirection)
{
	float directionAngleCos = 1.0 - dot(fromLightVector, spotDirection);
	float spotAngleCos = 1.0 - cos(radians(spotAngle / 2.0));
	return 1 - smoothstep(spotAngleCos * FALLOFF, spotAngleCos, directionAngleCos);
}