// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 pos : SV_POSITION;
	float3 color : COLOR0;
};

TextureCube baseTexture : register(t0);

SamplerState filters[1] : register(s0);

// A pass-through function for the (interpolated) color data.
float4 main(PixelShaderInput input) : SV_TARGET
{
	float4 colors = baseTexture.Sample(filters[0], input.color.xyz);

	return colors;
}
