cbuffer PerFrame : register(b0)
{
	matrix view;
	matrix proj;
	matrix world;
	matrix worldView;
	matrix worldViewProj;
	matrix invTransWV;
	matrix ortho;
	matrix toTex;
	float3 viewPos;
};



struct VertexIn
{
	float4 pos			: POSITION;
	float2 tex			: TEXCOORD;
	float3 normal		: NORMAL;
};

struct VertexOut
{
	float4 pos			: SV_POSITION;
	float2 tex			: TEXCOORD;
	float3 normal		: NORMAL;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;
	vout = vin;

	return vout;
}