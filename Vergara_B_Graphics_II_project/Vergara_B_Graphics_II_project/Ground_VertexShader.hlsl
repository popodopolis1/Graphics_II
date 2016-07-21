// A constant buffer that stores the three basic column-major matrices for composing geometry.

// Per-vertex data used as input to the vertex shader.
struct VertexShaderInput
{
	float3 position : POSITION;
	float3 tc : TEXTURE_COORDINATES;
	float3 normals : NORMALS;
	float3 worldPosition : WPOS;
};

// Per-pixel color data passed through the pixel shader.
struct PixelShaderInput
{
	float4 position : SV_POSITION;
	float4 tc : TEXTURE_COORDINATES;
	float4 normals : NORMALS;
	float4 worldPosition : WPOS;
};

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

// Simple shader to do vertex processing on the GPU.
PixelShaderInput main(VertexShaderInput input)
{
	PixelShaderInput output;
	float4 pos = float4(input.position, 1.0f);


	pos = mul(pos, model);

	output.worldPosition = pos;

	pos = mul(pos, view);
	pos = mul(pos, projection);
	output.position = pos;

	float4 tex = { input.tc, 1 };

	float4 norm = { input.normals, 0 };

	output.tc = tex;

	output.normals = norm;
	output.normals = normalize(output.normals);

	return output;
}
