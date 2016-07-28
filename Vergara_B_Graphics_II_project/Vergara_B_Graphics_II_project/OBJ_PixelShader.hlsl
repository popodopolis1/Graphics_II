struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 tc : TEXTURE_COORDINATES;
	float4 normals : NORMALS;
	float4 worldPosition : WPOS;
};

Texture2D baseTexture : register(t0);

SamplerState filters[1] : register(s0);

cbuffer LIGHTING : register(b0)
{
	float4 ambient;
	float3 direction;
	float padding;

	float4 color;
	float3 direction2;
	float lightRadius;
	float3 position;
	float padding2;

	float4 color2;
	float3 direction3;
	float coneRatio;
	float3 position2;
	float4 cameraPosition;
	float padding3;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
	//float4 textureC = float4(tc, 0, 1);
	//textureC = baseTexture.Sample(filters[0], input.tc);

	//float dirRatio = saturate(dot(-direction, input.normals));
	//float4 dirResult = dirRatio * ambient;
	//
	//float3 lightDirection = normalize(position.xyz - input.worldPosition.xyz);
	//float lightRatio = saturate(dot(lightDirection, input.normals));
	//float attenuation = 1.0f - saturate(length(position.xyz - input.worldPosition.xyz) / lightRadius);
	//float4 pointResult = lightRatio * attenuation * color;
	//
	//float3 specularDirection = normalize(cameraPosition.xyz - input.worldPosition.xyz);
	//float3 halfVector = normalize((lightDirection)+specularDirection);
	//float pIntensity = pow(saturate(dot(input.normals, normalize(halfVector))), 64.0f);
	//float4 specularResult = color * attenuation * pIntensity;
	//
	//float3 lightDirection2 = normalize(position2.xyz - input.worldPosition.xyz);
	//float surfaceRatio = saturate(dot(-lightDirection2.xyz, direction3.xyz));
	//float spotlightRatio = saturate(dot(lightDirection2, input.normals));
	//float innerCone = cos(10.0f * (3.1415f / 180.0f));
	//float outerCone = cos(20.0f * (3.1415f / 180.0f));
	//float attenuation2 = 1.0f - saturate((innerCone - surfaceRatio) / (innerCone - outerCone));
	//float4 spotlightResult = spotlightRatio * color2 * attenuation2;
	//
	//float3 specularDirection2 = normalize(cameraPosition.xyz - input.worldPosition.xyz);
	//float3 halfVector2 = normalize((lightDirection2)+specularDirection2);
	//float sIntensity = pow(saturate(dot(input.normals, normalize(halfVector2))), 64.0f);
	//float4 specularResult2 = color2 * attenuation2 * sIntensity;
	return input.tc;
	//return textureC * saturate(dirResult + pointResult + specularResult + spotlightResult + specularResult2);
}
