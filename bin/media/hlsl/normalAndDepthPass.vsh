cbuffer SSAOBuffer : register(b10)
{
	float farClip;
	float nearClip;
	int kernal_size;
	int width;
	int height;
}

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
	float2 tex			: TEXCOORD;
	float3 normal		: NORMAL;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;

	float4 pos = mul(vin.pos, worldView);
	vout.normal = vin.normal;
	vout.pos = mul(pos, proj);
	vout.pos.z = pos.z - (nearClip / farClip) - nearClip;
	vout.tex = vin.tex;

	

	

	return vout;
}