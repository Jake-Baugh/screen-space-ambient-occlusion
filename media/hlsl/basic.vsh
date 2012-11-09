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
	float2 tex			: TEXCOORD2;
	float4 ssaotex			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 viewDir		: TEXCOORD1;
};

VertexOut main(VertexIn vin)
{
	VertexOut vout;

	vout.pos		= mul(vin.pos, worldViewProj);
	vout.ssaotex	= mul(vout.pos, toTex);
	vout.normal		= mul(vin.normal, invTransWV);
	vout.tex		= vin.tex;

	// Calculate the position of the vertex in the world.
    float4 worldPosition = mul(vin.pos, world);

    // Determine the viewing direction based on the position of the camera and the position of the vertex in the world.
    vout.viewDir = viewPos.xyz - worldPosition.xyz;
	
    // Normalize the viewing direction vector.
    vout.viewDir = normalize(vout.viewDir);

	return vout;
}