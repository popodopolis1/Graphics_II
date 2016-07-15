//#pragma pack_matrix(row_major)

struct INPUT_VERTEX
{
	float4 coordinate : POSITION;
	float4 color : COLOR0;
	float3 norm : NORMALS;
};

struct OUTPUT_VERTEX
{
	float4 projectedCoordinate : SV_POSITION;
	float4 colorOut : COLOR0;
	float4 normOut : NORMALS;
};

cbuffer ModelViewProjectionConstantBuffer : register(b0)
{
	matrix model;
	matrix view;
	matrix projection;
};

OUTPUT_VERTEX main(INPUT_VERTEX fromVertexBuffer)
{
	OUTPUT_VERTEX sendToRasterizer = (OUTPUT_VERTEX)0;

	float4 localH = fromVertexBuffer.coordinate;

	localH = mul(localH, model);

	float4 normTemp = { fromVertexBuffer.norm, 0 };
	localH = mul(localH, view);
	localH = mul(localH, projection);

	sendToRasterizer.projectedCoordinate = localH;
	sendToRasterizer.normOut = normTemp;

	sendToRasterizer.colorOut = fromVertexBuffer.color;
	return sendToRasterizer;
}