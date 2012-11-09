#ifndef _shaderInfo_h_
#define _shaderInfo_h_

#include <D3D11.h>
#include <D3DX11.h>
#include <D3Dcompiler.h>
#include <string>
#include <vector>


#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "dxguid.lib")

typedef std::vector<D3D11_SIGNATURE_PARAMETER_DESC> ParamList;
typedef std::vector<D3D11_SHADER_INPUT_BIND_DESC> BoundResources;
typedef std::vector<D3D11_SHADER_INPUT_BIND_DESC> SamplerList;
typedef std::vector<D3D11_SHADER_INPUT_BIND_DESC> TextureList;

typedef struct Variable
{
	D3D11_SHADER_VARIABLE_DESC var;
	D3D11_SHADER_TYPE_DESC type;
} Variable;

typedef struct Cbuffer
{
	D3D11_SHADER_INPUT_BIND_DESC input_bind_desc;
	D3D11_SHADER_BUFFER_DESC buffer_desc;
	std::vector<Variable*> vars;
} Cbuffer;

typedef std::vector<Cbuffer*> CBufferList;

class ShaderInfo
{
public:
	ShaderInfo();
	~ShaderInfo();

	void ReflectShader(ID3D10Blob* shader);
	void CreateInputLayout(D3D11_INPUT_ELEMENT_DESC* layout, UINT elementCount,
		bool perVertex = true, UINT instanceDataStepRate = 0);
	int GetResourceBindPoint(const std::string* name);
	DXGI_FORMAT GetDXGIFormat(D3D11_SIGNATURE_PARAMETER_DESC pd);
private:
	ID3D11ShaderReflection* m_reflector;
public:
	D3D11_SHADER_DESC shaderDesc;
	CBufferList m_cbuffers;
	SamplerList m_samplers;
	TextureList m_textures;
	ParamList m_inputParams;
	ParamList m_outputParams;
	BoundResources m_resources;

};

#endif