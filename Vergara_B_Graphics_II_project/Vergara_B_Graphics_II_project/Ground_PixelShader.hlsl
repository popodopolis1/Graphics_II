// Per-pixel color data passed through the pixel shader.
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
	float pad;
}

float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 textureC;
	textureC = baseTexture.Sample(filters[0], input.tc);

	float dirRatio = saturate(dot(-direction, input.normals));
	float4 dirResult = dirRatio * ambient;

	return textureC * saturate(dirResult);
}
