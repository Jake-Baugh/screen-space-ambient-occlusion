cbuffer SSAOBuffer : register(b10)
{
	float farClip;
	float nearClip;
	float kernal_size;
	float width;
	float height;
	float2 noiseScale;
	float3 frustumCorner;
};

cbuffer PerFrame : register(b0)
{
	matrix view;
	matrix proj;
	matrix world;
	matrix worldView;
	matrix worldViewProj;
	matrix invTransWV;
	matrix othro;
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
	float2 tex			: TEXCOORD0;
	float3 view_ray		: TEXCOORD1;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;

	vout.pos = vin.pos;
	vout.tex = vin.tex;
	vout.view_ray = float3(frustumCorner.x * vout.pos.x, frustumCorner.y * vout.pos.y, frustumCorner.z);

	return vout;
}