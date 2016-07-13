//#pragma pack_matrix(row_major)

struct INPUT_PIXEL
{
	float4 projectedCoordinate : SV_POSITION;
	float4 colorOut : COLOR;
	float4 normOut : NORMALS;
};


float4 main(INPUT_PIXEL fromVS) : SV_TARGET
{
return fromVS.colorOut;
}
