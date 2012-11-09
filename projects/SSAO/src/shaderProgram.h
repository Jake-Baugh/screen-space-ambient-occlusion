#ifndef _shader_h_
#define _shader_h_

#include <D3D11.h>
#include <D3Dcompiler.h>
#include <string>
#include <vector>
#include <map>
#include "util.h"
#include "ShaderInfo.h"

#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

/*
program->InitializeBuffers()
program->BindResources();

program->shaderVars->SetMatrix("gWorld", world);
program->shaderVars->SetVector("gLightPos", lightpos);
program->shaderVars->SetScalar("shininess", shiny);
program->shaderVars->SetShaderResource("shadowMap", shadowMap);
program->shaderVars->SetSampler("texSampler", sampler);
	//Set* will find the params, even ones in constant buffers
program->UpdateBuffers();
	//Check if a constant buffer has been marked dirty
	//For all dirty buffers, remap their variables
*/

class ShaderInfo;


//-------------------------------------------------------------------------------------
// ShaderProgram
//-------------------------------------------------------------------------------------
class ShaderProgram
{
public:
	ShaderProgram(ID3D11Device* device, ID3D11DeviceContext* dc);
	~ShaderProgram();

	void CompileShaders(std::wstring vs_fpath, std::wstring ps_fpath);
	void SetActive();
private:
	void HandleCompileError(ID3D10Blob* errorMessage);
private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader*  m_pixelShader;

	ID3D11InputLayout* m_inputLayout;

	std::wstring m_vsFpath;
	std::wstring m_psFpath;

	ShaderInfo* m_vsInfo;
	ShaderInfo* m_psInfo;

	ID3D11Device*			m_device;
	ID3D11DeviceContext*	m_deviceContext;
};



#endif